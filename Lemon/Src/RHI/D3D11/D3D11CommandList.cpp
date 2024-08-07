#include "LemonPCH.h"
#include "D3D11CommandList.h"
#include <glm/gtc/type_ptr.hpp>
#include "RHI/RHIResources.h"
#include "RHI/RHISwapChain.h"
#include "RHI/RHIStaticStates.h"
#include <DirectXMath.h>
using namespace DirectX;

namespace Lemon
{
	D3D11CommandList::D3D11CommandList(D3D11DynamicRHI* D3D11RHI, Renderer* renderer)
		: RHICommandList(renderer)
		, m_D3D11RHI(D3D11RHI)
	{
		// RCM_CCW is FRONT face
		// RCM_CW is back face
		m_DefaultRasterizerState = TStaticRasterizerState<RFM_Solid, RCM_Back>::CreateRHI();
		m_DefaultDepthStencilState = TStaticDepthStencilState<>::CreateRHI();
		
	}

	//===================================RHI RenderCommand Helper function==================================//

	void D3D11CommandList::RHIClearRenderTarget(Ref<RHITexture2D> renderTarget, glm::vec4 backgroundColor,
            Ref<RHITexture2D> depthStencilTarget, float depthClear, float stencilClear)
	{
		const void* rtvs[1] = { renderTarget->GetNativeRenderTargetView(0) };
		m_D3D11RHI->GetDeviceContext()->ClearRenderTargetView(static_cast<ID3D11RenderTargetView*>(renderTarget->GetNativeRenderTargetView(0)), glm::value_ptr(backgroundColor));
		if(depthStencilTarget)
		{
			m_D3D11RHI->GetDeviceContext()->ClearDepthStencilView(static_cast<ID3D11DepthStencilView*>(depthStencilTarget->GetNativeDepthStencilView()),
				D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthClear, stencilClear);
		}
	}

	void D3D11CommandList::RHIClearRenderTarget(Ref<RHITextureCube> renderTargets, int renderTargetIndex, glm::vec4 backgroundColor,
		Ref<RHITexture2D> depthStencilTarget, float depthClear , float stencilClear) 
	{
		const void* rtvs[1] = { renderTargets->GetNativeRenderTargetView(renderTargetIndex) };
		m_D3D11RHI->GetDeviceContext()->ClearRenderTargetView(
			static_cast<ID3D11RenderTargetView*>(renderTargets->GetNativeRenderTargetView(renderTargetIndex)), 
			glm::value_ptr(backgroundColor));
		if (depthStencilTarget)
		{
			m_D3D11RHI->GetDeviceContext()->ClearDepthStencilView(static_cast<ID3D11DepthStencilView*>(depthStencilTarget->GetNativeDepthStencilView()),
				D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthClear, stencilClear);
		}
	}

	void D3D11CommandList::RHIClearRenderTarget(std::vector<Ref<RHITexture2D>> colorTargets, glm::vec4 backgroundColor,
		Ref<RHITexture2D> depthStencilTarget, float depthClear, float stencilClear)
	{
		for (int i = 0; i < colorTargets.size(); i++)
		{
			if (colorTargets[i])
			{
				m_D3D11RHI->GetDeviceContext()->ClearRenderTargetView(
					static_cast<ID3D11RenderTargetView*>(colorTargets[i]->GetNativeRenderTargetView(0)),
					glm::value_ptr(backgroundColor));
			}
		}
	
		if (depthStencilTarget)
		{
			m_D3D11RHI->GetDeviceContext()->ClearDepthStencilView(static_cast<ID3D11DepthStencilView*>(depthStencilTarget->GetNativeDepthStencilView()),
				D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthClear, stencilClear);
		}
	}

	void D3D11CommandList::SetRenderTarget(Ref<RHISwapChain> swapChain, float depthClear /*= 1.0f*/, float stencilClear /*= 0*/)
	{
		const void* rtvs[1] = { swapChain->GetRHIRenderTargetView() };

		ID3D11RenderTargetView* renderTargetView = static_cast<ID3D11RenderTargetView*>(swapChain->GetRHIRenderTargetView());
		ID3D11DepthStencilView* depthStencilView = static_cast<ID3D11DepthStencilView*>(swapChain->GetRHIDepthStencilView());
		m_D3D11RHI->GetDeviceContext()->OMSetRenderTargets
		(
			1,
			&renderTargetView,
			depthStencilView
		); 
		m_D3D11RHI->GetDeviceContext()->ClearRenderTargetView(renderTargetView, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
		if (depthStencilView)
		{
			m_D3D11RHI->GetDeviceContext()->ClearDepthStencilView(depthStencilView,
				D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthClear, stencilClear);
		}
	}

	void D3D11CommandList::SetGraphicsPipelineState(const GraphicsPipelineStateInitializer& GraphicsPSOInit)
	{
		D3D11VertexShader* vertexShader = D3D11ResourceCast(GraphicsPSOInit.BoundShaderState.VertexShaderRHI.get());
		D3D11PixelShader* pixelShader = D3D11ResourceCast(GraphicsPSOInit.BoundShaderState.PixelShaderRHI.get());
		D3D11VertexDeclaration* vertexDeclaration = D3D11ResourceCast(GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI.get());

		if (m_CurrentGraphicsPipelineState == GraphicsPSOInit)
		{
			return;
		}

		m_CurrentGraphicsPipelineState = GraphicsPSOInit;
		m_CurrentPrimitiveType = GraphicsPSOInit.PrimitiveType;

		// InputLayout
		if (vertexDeclaration)
		{
			m_D3D11RHI->GetDeviceContext()->IASetInputLayout(vertexDeclaration->m_InputLayout);
		}
		// Vertex shader
		if (vertexShader)
		{
			m_D3D11RHI->GetDeviceContext()->VSSetShader(vertexShader->m_Resource, nullptr, 0);
		}
		// Pixel shader
		if (pixelShader)
		{
			m_D3D11RHI->GetDeviceContext()->PSSetShader(pixelShader->m_Resource, nullptr, 0);
		}
		
		//Blend State
		if (GraphicsPSOInit.BlendState)
		{
			if (void* resource = GraphicsPSOInit.BlendState->GetNativeResource())
			{
				float blendFactor = 1.0f;
				FLOAT blendFactors[4] = { blendFactor, blendFactor, blendFactor, blendFactor };

				m_D3D11RHI->GetDeviceContext()->OMSetBlendState
                (
                    static_cast<ID3D11BlendState*>(const_cast<void*>(resource)),
                    blendFactors,
                    0xffffffff
                );
			}
		}
		
		//Depth Stencil State
		if (GraphicsPSOInit.DepthStencilState)
		{
			if (void* resource = GraphicsPSOInit.DepthStencilState->GetNativeResource())
			{
				m_D3D11RHI->GetDeviceContext()->OMSetDepthStencilState(static_cast<ID3D11DepthStencilState*>(const_cast<void*>(resource)), 1);
			}
		}else
		{
			if (void* resource = m_DefaultDepthStencilState->GetNativeResource())
			{
				m_D3D11RHI->GetDeviceContext()->OMSetDepthStencilState(static_cast<ID3D11DepthStencilState*>(const_cast<void*>(resource)), 1);
			}
		}
		
		//Rasterizer State
		if (GraphicsPSOInit.RasterizerState)
		{
			if (void* resource = GraphicsPSOInit.RasterizerState->GetNativeResource())
			{
				m_D3D11RHI->GetDeviceContext()->RSSetState(static_cast<ID3D11RasterizerState*>(const_cast<void*>(resource)));
			}
		}
		else
		{
			if (void* resource = m_DefaultRasterizerState->GetNativeResource())
			{
				m_D3D11RHI->GetDeviceContext()->RSSetState(static_cast<ID3D11RasterizerState*>(const_cast<void*>(resource)));
			}
		}
		
		//PrimitiveTopology
		if (GraphicsPSOInit.PrimitiveType == EPrimitiveType::PT_TriangleList)
		{
			m_D3D11RHI->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
		else if(GraphicsPSOInit.PrimitiveType == EPrimitiveType::PT_LineList)
		{
			m_D3D11RHI->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		}
		
	}

	void D3D11CommandList::SetRenderTarget(Ref<RHITexture2D> colorTarget, Ref<RHITexture2D> depthTarget)
	{
		// render targets
		void* depthStencil = nullptr;
		if(depthTarget)
		{
			depthStencil = static_cast<ID3D11DepthStencilView*>(depthTarget->GetNativeDepthStencilView());
		}

		void* renderTargetViews[4];
		renderTargetViews[0] = nullptr;
		renderTargetViews[1] = nullptr;
		renderTargetViews[2] = nullptr;
		renderTargetViews[3] = nullptr;
		
		if (colorTarget)
		{
			renderTargetViews[0] = static_cast<ID3D11RenderTargetView*>(colorTarget->GetNativeRenderTargetView(0));
		}

		m_D3D11RHI->GetDeviceContext()->OMSetRenderTargets
		(
			4,
			reinterpret_cast<ID3D11RenderTargetView* const*>(renderTargetViews),
			static_cast<ID3D11DepthStencilView*>(depthStencil)
		);
	}

	void D3D11CommandList::SetRenderTarget(Ref<RHITextureCube> colorTargets, int colorTargetIndex, Ref<RHITexture2D> depthTarget)
	{
		// render targets
		void* depthStencil = nullptr;
		if (depthTarget)
		{
			depthStencil = static_cast<ID3D11DepthStencilView*>(depthTarget->GetNativeDepthStencilView());
		}

		if (colorTargets)
		{
			void* renderTargetViews[1];
			renderTargetViews[0] = static_cast<ID3D11RenderTargetView*>(colorTargets->GetNativeRenderTargetView(colorTargetIndex));
			
			m_D3D11RHI->GetDeviceContext()->OMSetRenderTargets
			(
				1,
				reinterpret_cast<ID3D11RenderTargetView *const*>(renderTargetViews),
				static_cast<ID3D11DepthStencilView*>(depthStencil)
			);
		}
	}


	void D3D11CommandList::SetRenderTarget(std::vector<Ref<RHITexture2D>> colorTargets, Ref<RHITexture2D> depthTarget /*= nullptr*/)
	{
		// render targets
		void* depthStencil = nullptr;
		if (depthTarget)
		{
			depthStencil = static_cast<ID3D11DepthStencilView*>(depthTarget->GetNativeDepthStencilView());
		}

		if (colorTargets.size())
		{
			void* renderTargetViews[4];
			renderTargetViews[0] = static_cast<ID3D11RenderTargetView*>(colorTargets[0]->GetNativeRenderTargetView(0));
			renderTargetViews[1] = static_cast<ID3D11RenderTargetView*>(colorTargets[1]->GetNativeRenderTargetView(0));
			renderTargetViews[2] = static_cast<ID3D11RenderTargetView*>(colorTargets[2]->GetNativeRenderTargetView(0));
			renderTargetViews[3] = static_cast<ID3D11RenderTargetView*>(colorTargets[3]->GetNativeRenderTargetView(0));

			m_D3D11RHI->GetDeviceContext()->OMSetRenderTargets
			(
				colorTargets.size(),
				reinterpret_cast<ID3D11RenderTargetView* const*>(renderTargetViews),
				static_cast<ID3D11DepthStencilView*>(depthStencil)
			);
		}
	}


	void D3D11CommandList::SetViewport(const Viewport& viewport)
	{
		D3D11_VIEWPORT d3d11Viewport = {};
		d3d11Viewport.TopLeftX = viewport.X;
		d3d11Viewport.TopLeftY = viewport.Y;
		d3d11Viewport.Width = viewport.Width;
		d3d11Viewport.Height = viewport.Height;
		d3d11Viewport.MinDepth = viewport.DepthMin;
		d3d11Viewport.MaxDepth = viewport.DepthMax;
		m_D3D11RHI->GetDeviceContext()->RSSetViewports(1, &d3d11Viewport);
	}

	void D3D11CommandList::SetIndexBuffer(const RHIIndexBuffer* indexBuffer)
	{
		if (!indexBuffer || !indexBuffer->GetNativeResource())
		{
			LEMON_CORE_ERROR("Invalid SetIndexBuffer");
			return;
		}
		// if cache .we don't need set it again

		m_D3D11RHI->GetDeviceContext()->IASetIndexBuffer
		(
			static_cast<ID3D11Buffer*>(indexBuffer->GetNativeResource()),
			/*buffer->Is16Bit()*/false ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT,
			0
		);
	}

	void D3D11CommandList::SetVertexBuffer(int streamIndex, const RHIVertexBuffer* vertexBuffer)
	{
		if (!vertexBuffer || !vertexBuffer->GetNativeResource())
		{
			LEMON_CORE_ERROR("Invalid SetBufferVertex");
			return;
		}

		// if cache .we don't need set it again
		if (!m_CurrentGraphicsPipelineState.IsValid())
		{
			LEMON_CORE_ERROR("Empty Bound States");
			return;
		}
		ID3D11Buffer* buffer = static_cast<ID3D11Buffer*>(vertexBuffer->GetNativeResource());
		uint32_t stride = m_CurrentGraphicsPipelineState.BoundShaderState.VertexDeclarationRHI->m_StreamStrides[streamIndex];
		uint32_t offset = 0;

		m_D3D11RHI->GetDeviceContext()->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	}

	void D3D11CommandList::DrawIndexPrimitive(uint32_t VertexOffset, uint32_t IndexOffset, uint32_t NumPrimitives, uint32_t FirstInstance/* = 0*/, uint32_t NumInstances/* = 1*/)
	{
		uint32_t indexCount = GetIndexCountForPrimitiveCount(NumPrimitives, m_CurrentPrimitiveType);
		if (NumInstances > 1 || FirstInstance != 0)
		{
			m_D3D11RHI->GetDeviceContext()->DrawIndexedInstanced(indexCount, NumInstances, IndexOffset, VertexOffset, FirstInstance);
		}
		else
		{
			m_D3D11RHI->GetDeviceContext()->DrawIndexed(indexCount, IndexOffset, VertexOffset);
		}
	}

	void D3D11CommandList::SetUniformBuffer(uint32_t slot, EUniformBufferUsageScopeType scopeType, const RHIUniformBufferBaseRef& uniformBuffer)
	{
		void* resource = uniformBuffer ? uniformBuffer->GetNativeResource() : nullptr;
		const void* resourceArray[1] = { resource };
		uint32_t resourceCount = 1;

		if (scopeType & EUniformBufferUsageScope::UBUS_Vertex)
		{
			m_D3D11RHI->GetDeviceContext()->VSSetConstantBuffers(
                static_cast<UINT>(slot),
                static_cast<UINT>(resourceCount),
                static_cast<ID3D11Buffer *const*>(resourceCount > 1 ? resource : &resourceArray)
            );
		}
		if (scopeType & EUniformBufferUsageScope::UBUS_Pixel)
		{
			m_D3D11RHI->GetDeviceContext()->PSSetConstantBuffers(
                static_cast<UINT>(slot),
                static_cast<UINT>(resourceCount),
                static_cast<ID3D11Buffer * const*>(resourceCount > 1 ? resource : &resourceArray)
            );
		}
	}
	
	void D3D11CommandList::Flush()
	{
		m_D3D11RHI->GetDeviceContext()->Flush();
	}

	void D3D11CommandList::SetSamplerState(uint32_t slot,const Ref<RHISamplerState>& samplerState)
	{
		if(samplerState)
		{
			void* resource = samplerState->GetNativeResource();
			const void* resourceArray[1] = { resource };
			m_D3D11RHI->GetDeviceContext()->PSSetSamplers
			(
				slot,
				1,
				reinterpret_cast<ID3D11SamplerState * const*>(&resourceArray)
			);
		}
		
	}

	void D3D11CommandList::SetTexture(uint32_t slot, const Ref<RHITexture>& texture)
	{
		void* resource = nullptr;
		if (texture)
		{
			resource = texture->GetNativeShaderResourceView();
		}

		const void* resourceArray[1] = { resource };
		m_D3D11RHI->GetDeviceContext()->PSSetShaderResources
		(
           static_cast<UINT>(slot),
           static_cast<UINT>(1),
            reinterpret_cast<ID3D11ShaderResourceView * const*>(&resourceArray)
        );
	}

	void D3D11CommandList::SetMipTexture(Ref<RHITextureCube> targetTex, int mipIndex, int mipWidth, int mipHeight, std::vector<Ref<RHITexture2D>> mipTextures)
	{
		m_D3D11RHI->SetMipTexture(targetTex, mipIndex, mipWidth, mipHeight, mipTextures);
	}

}