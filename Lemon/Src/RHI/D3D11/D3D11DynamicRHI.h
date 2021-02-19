#pragma once
#include "RHI/DynamicRHI.h"
#include "RHI/D3D11/D3D11RHI.h"
#include <wrl/client.h>
#include <glm/glm.hpp>

namespace Lemon
{

	class D3D11DynamicRHI : public DynamicRHI
	{
	public:
		virtual void Init() override;

		/** Shutdown the RHI; handle shutdown and resource destruction before the RHI's actual destructor is called (so that all resources of the RHI are still available for shutdown). */
		virtual void Shutdown()  override;


		//===================Begin RHI Methods ==================//
		virtual Ref<RHISwapChain> RHICreateSwapChain(void* windowHandle, const uint32_t width, const uint32_t height, const ERHIFormat format) override;


		//========Just Debug
		virtual void RHIClearRenderTarget(Ref<RHISwapChain> swapChain, glm::vec4 backgroundColor) override;

		//===================End RHI Methods ==================//

		ID3D11Device5* GetDevice() const { return m_Direct3DDevice.Get(); }
		ID3D11DeviceContext4* GetDeviceContext() const { return m_Direct3DDeviceIMContext.Get(); }

	private:
		void InitD3DDevice();

	private:
		ComPtr<ID3D11Device5> m_Direct3DDevice;
		ComPtr<ID3D11DeviceContext4> m_Direct3DDeviceIMContext;




	};
}