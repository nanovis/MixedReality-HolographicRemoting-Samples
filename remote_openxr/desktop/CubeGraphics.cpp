//*********************************************************
//    Copyright (c) Microsoft. All rights reserved.
//
//    Apache 2.0 License
//
//    You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
//    implied. See the License for the specific language governing
//    permissions and limitations under the License.
//
//*********************************************************



// This class contains DirectX and OpenGL Code.



#define GLEW_STATIC

#include "pch.h"
#include "OpenXrProgram.h"
#include "DxUtility.h"
#include"OpenGLProgram.h"
#include <iostream>

namespace {
    namespace CubeShader {
       //DirectX 
        struct Vertex {
            XrVector3f Position; 
            XrVector3f Color;
            XrVector2f tex;
        };                      //tex

        constexpr XrVector3f Red{1, 0, 0};
        constexpr XrVector3f Green{0, 1, 0};
        constexpr XrVector3f Blue{0, 0, 1};
        constexpr XrVector3f Yellow{0, 1, 1};
        constexpr XrVector3f Purple{1, 0, 1};
        constexpr XrVector3f Brown{1, 1, 0};
        constexpr XrVector3f Black{0, 0, 0};
        constexpr XrVector3f White{1, 1, 1};

        // Vertices for a 1x1x1 meter cube. (Left/Right, Top/Bottom, Front/Back)
       constexpr XrVector3f LBB{-0.5f, -0.5f, -0.5f};
        constexpr XrVector3f LBF{-1.0f, -1.0f, 0.5f};
        constexpr XrVector3f LTB{-0.5f, 0.5f, -0.5f};
        constexpr XrVector3f LTF{-1.0f, 1.0f, 0.5f};


        constexpr XrVector3f RBB{0.5f, -0.5f, -0.5f};
        constexpr XrVector3f RBF{1.0f, -1.0f, 0.5f};
        constexpr XrVector3f RTB{0.5f, 0.5f, -0.5f};
        constexpr XrVector3f RTF{1.0f, 1.0f, 0.5f};

        //texture Cordi
        constexpr XrVector2f LU {0.0f,0.0f};
        constexpr XrVector2f RU{1.0f, 0.0f};
        constexpr XrVector2f RL{1.0f, 1.0f};
        constexpr XrVector2f LL{0.0f, 1.0f};

#define CUBE_SIDE(V1, V2, V3, V4, V5, V6, COLOR, TEXTURE) \
    {V1, COLOR, TEXTURE}, {V2, COLOR, TEXTURE}, {V3, COLOR, TEXTURE}, {V4, COLOR, TEXTURE}, {V5, COLOR, TEXTURE}, {V6, COLOR, TEXTURE},

        constexpr Vertex c_cubeVertices[] = {

            {LBF, Blue, LU}, // 0
            {LTF, Yellow,LL},//1
            {RBF, Purple, RU}, //2
            {RTF, White, RL}, // 3
            };

        constexpr unsigned short c_cubeIndices[] = {
            0,1,3,
            0,3,2,
       };

        struct ModelConstantBuffer {
            DirectX::XMFLOAT4X4 Model;
        };

        struct ViewProjectionConstantBuffer {
            DirectX::XMFLOAT4X4 ViewProjection[2];
        };

        struct ColorFilterConstantBuffer {
            DirectX::XMFLOAT4 colorFilter;
        };

        constexpr uint32_t MaxViewInstance = 2;

        // Separate entrypoints for the vertex and pixel shader functions.
        constexpr char ShaderHlsl[] = R"_(
          Texture2D texture_: register(t0);
          SamplerState sampler_: register(s0);
           struct VSOutput {
                float4 Pos : SV_POSITION;
                float3 Color : COLOR0;
                float2 Texcoord  : TEXCOORD0; 
                uint viewId : SV_RenderTargetArrayIndex;
            };
            struct VSInput {
                float3 Pos : POSITION;
                float3 Color : COLOR0;
                float2 Texcoord  : TEXCOORD0; 
                uint instId : SV_InstanceID;
            };
            cbuffer ModelConstantBuffer : register(b0) {
                float4x4 Model;
            };
            cbuffer ViewProjectionConstantBuffer : register(b1) {
                float4x4 ViewProjection[2];
            };
            cbuffer ColorFilterConstantBuffer: register(b0) {
                float4 colorFilter;
            };
            VSOutput MainVS(VSInput input) {
                VSOutput output;
                output.Pos = float4(input.Pos,1);
                output.Color = input.Color;
                // Map left part of the texture to the left eye and right part of the texture to the right eye
                if (input.instId == 0) { // left eye
                    output.Texcoord = float2(input.Texcoord.x * 0.5, input.Texcoord.y);
                } else { // right eye
                    output.Texcoord = float2(0.5 + input.Texcoord.x * 0.5, input.Texcoord.y);
                }
                output.viewId = input.instId;
                return output;
            }
            float4 MainPS(VSOutput input) : SV_TARGET {
                return texture_.Sample(sampler_ , input.Texcoord);
            }
            )_";
    } // namespace CubeShader

    struct CubeGraphics : sample::IGraphicsPluginD3D11 {
       
        OpenGLProgram* getOpenGLProgramPtr() override {
            return m_openGLProgram;
        };
        //---------------------------------------------------------------------------------------------------------------------------
        // DirectX methods 

        ID3D11Device* InitializeDevice(LUID adapterLuid,
                                       const std::vector<D3D_FEATURE_LEVEL>& featureLevels) override {
            const winrt::com_ptr<IDXGIAdapter1> adapter = sample::dx::GetAdapter(adapterLuid);
            sample::dx::CreateD3D11DeviceAndContext(adapter.get(), featureLevels, m_device.put(), m_deviceContext.put());
            InitializeD3DResources();
            return m_device.get();
        }
        winrt::com_ptr<ID3D11DeviceContext> getDeviceContext() override {
            return m_deviceContext;
        }
        void InitializeD3DResources() {
            const winrt::com_ptr<ID3DBlob> vertexShaderBytes = sample::dx::CompileShader(CubeShader::ShaderHlsl, "MainVS", "vs_5_0");
            CHECK_HRCMD(m_device->CreateVertexShader(
                vertexShaderBytes->GetBufferPointer(), vertexShaderBytes->GetBufferSize(), nullptr, m_vertexShader.put()));

            const winrt::com_ptr<ID3DBlob> pixelShaderBytes = sample::dx::CompileShader(CubeShader::ShaderHlsl, "MainPS", "ps_5_0");
            CHECK_HRCMD(m_device->CreatePixelShader(
                pixelShaderBytes->GetBufferPointer(), pixelShaderBytes->GetBufferSize(), nullptr, m_pixelShader.put()));

            const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},

            };

            CHECK_HRCMD(m_device->CreateInputLayout(vertexDesc,
                                                    (UINT)std::size(vertexDesc),
                                                    vertexShaderBytes->GetBufferPointer(),
                                                    vertexShaderBytes->GetBufferSize(),
                                                    m_inputLayout.put()));

            const CD3D11_BUFFER_DESC modelConstantBufferDesc(sizeof(CubeShader::ModelConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
            CHECK_HRCMD(m_device->CreateBuffer(&modelConstantBufferDesc, nullptr, m_modelCBuffer.put()));

            const CD3D11_BUFFER_DESC viewProjectionConstantBufferDesc(sizeof(CubeShader::ViewProjectionConstantBuffer),
                                                                      D3D11_BIND_CONSTANT_BUFFER);
            CHECK_HRCMD(m_device->CreateBuffer(&viewProjectionConstantBufferDesc, nullptr, m_viewProjectionCBuffer.put()));

            const CD3D11_BUFFER_DESC colorFilterConstantBufferDesc(sizeof(CubeShader::ColorFilterConstantBuffer),
                                                                   D3D11_BIND_CONSTANT_BUFFER);
            CHECK_HRCMD(m_device->CreateBuffer(&colorFilterConstantBufferDesc, nullptr, m_colorFilterCBuffer.put()));

            const D3D11_SUBRESOURCE_DATA vertexBufferData{CubeShader::c_cubeVertices};
            const CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(CubeShader::c_cubeVertices), D3D11_BIND_VERTEX_BUFFER);
            CHECK_HRCMD(m_device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, m_cubeVertexBuffer.put()));

            const D3D11_SUBRESOURCE_DATA indexBufferData{CubeShader::c_cubeIndices};
            const CD3D11_BUFFER_DESC indexBufferDesc(sizeof(CubeShader::c_cubeIndices), D3D11_BIND_INDEX_BUFFER);
            CHECK_HRCMD(m_device->CreateBuffer(&indexBufferDesc, &indexBufferData, m_cubeIndexBuffer.put()));

            D3D11_FEATURE_DATA_D3D11_OPTIONS3 options;
            m_device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS3, &options, sizeof(options));
            CHECK_MSG(options.VPAndRTArrayIndexFromAnyShaderFeedingRasterizer,
                      "This sample requires VPRT support. Adjust sample shaders on GPU without VRPT.");

            CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(CD3D11_DEFAULT{});
            depthStencilDesc.DepthEnable = true;
            depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER;
            CHECK_HRCMD(m_device->CreateDepthStencilState(&depthStencilDesc, m_reversedZDepthNoStencilTest.put()));

           
        }

        const std::vector<DXGI_FORMAT>& SupportedColorFormats() const override {
            const static std::vector<DXGI_FORMAT> SupportedColorFormats = {
                DXGI_FORMAT_R8G8B8A8_UNORM,
                DXGI_FORMAT_B8G8R8A8_UNORM,
                DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
                DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
            };
            return SupportedColorFormats;
        }

        const std::vector<DXGI_FORMAT>& SupportedDepthFormats() const override {
            const static std::vector<DXGI_FORMAT> SupportedDepthFormats = {
                DXGI_FORMAT_D32_FLOAT,
                DXGI_FORMAT_D16_UNORM,
                DXGI_FORMAT_D24_UNORM_S8_UINT,
                DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
            };
            return SupportedDepthFormats;
        }
    
        void RenderView(const XrRect2Di& imageRect,
                        const float renderTargetClearColor[4],
                        const std::vector<xr::math::ViewProjection>& viewProjections,
                        DXGI_FORMAT colorSwapchainFormat,
                        ID3D11Texture2D* colorTexture,
                        DXGI_FORMAT depthSwapchainFormat,
                        ID3D11Texture2D* depthTexture,
                        const std::vector<const sample::Cube*>& cubes,
                        ID3D11Texture2D* m_dxColorbuffer) override {

            

          

            const uint32_t viewInstanceCount = (uint32_t)viewProjections.size();
            CHECK_MSG(viewInstanceCount <= CubeShader::MaxViewInstance,
                      "Sample shader supports 2 or fewer view instances. Adjust shader to accommodate more.")

            CD3D11_VIEWPORT viewport(
                (float)imageRect.offset.x, (float)imageRect.offset.y, (float)imageRect.extent.width, (float)imageRect.extent.height);
            m_deviceContext->RSSetViewports(1, &viewport);
       

            // Create RenderTargetView with the original swapchain format (swapchain image is typeless).
            winrt::com_ptr<ID3D11RenderTargetView> renderTargetView;
            const CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2DARRAY, colorSwapchainFormat);
            CHECK_HRCMD(m_device->CreateRenderTargetView(colorTexture, &renderTargetViewDesc, renderTargetView.put()));

         
            // Create a DepthStencilView with the original swapchain format (swapchain image is typeless)
            winrt::com_ptr<ID3D11DepthStencilView> depthStencilView;
            CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2DARRAY, depthSwapchainFormat);
            CHECK_HRCMD(m_device->CreateDepthStencilView(depthTexture, &depthStencilViewDesc, depthStencilView.put()));

            const bool reversedZ = viewProjections[0].NearFar.Near > viewProjections[0].NearFar.Far;
            const float depthClearValue = reversedZ ? 0.f : 1.f;

            // Clear swapchain and depth buffer. NOTE: This will clear the entire render target view, not just the specified view.
            m_deviceContext->ClearRenderTargetView(renderTargetView.get(), renderTargetClearColor);
            m_deviceContext->ClearDepthStencilView(depthStencilView.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthClearValue, 0);
            m_deviceContext->OMSetDepthStencilState(reversedZ ? m_reversedZDepthNoStencilTest.get() : nullptr, 0);

            ID3D11RenderTargetView* renderTargets[] = {renderTargetView.get()};
            m_deviceContext->OMSetRenderTargets((UINT)std::size(renderTargets), renderTargets, depthStencilView.get());

            /**/ ID3D11Buffer* const vsConstantBuffers[] = {m_modelCBuffer.get(), m_viewProjectionCBuffer.get()};
            m_deviceContext->VSSetConstantBuffers(0, (UINT)std::size(vsConstantBuffers), vsConstantBuffers);
            m_deviceContext->VSSetShader(m_vertexShader.get(), nullptr, 0);

            ID3D11Buffer* const psConstantBuffers[] = {m_colorFilterCBuffer.get()};
            m_deviceContext->PSSetConstantBuffers(0, (UINT)std::size(psConstantBuffers), psConstantBuffers);
            m_deviceContext->PSSetShader(m_pixelShader.get(), nullptr, 0);

                 D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc;
            ZeroMemory(&textureViewDesc, sizeof(textureViewDesc));
            textureViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            textureViewDesc.Texture2D.MostDetailedMip = 0;
              textureViewDesc.Texture2D.MipLevels = 1;

              // Bond the the texter to the resourcse view to allow Directx to render it.
            CHECK_HRCMD(m_device->CreateShaderResourceView(m_dxColorbuffer, &textureViewDesc, &textureView));

            
              D3D11_SAMPLER_DESC sampDesc = {};
              sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
              sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
              sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
              sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
              sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
              sampDesc.MinLOD = 0;
              sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
              CHECK_HRCMD(m_device->CreateSamplerState(&sampDesc, &samplerState));

              
              m_deviceContext->PSSetShaderResources(0, 1, &textureView);
              m_deviceContext->PSSetSamplers(0, 1, &samplerState);
           //=====================================================     
            // send view and projection matrices for both eyes to Opnegl 
            CubeShader::ViewProjectionConstantBuffer viewProjectionCBufferData{};

            // Set view projection matrix for left view
            m_openGLProgram->g_matView_0 = xr::math::LoadInvertedXrPose(viewProjections[0].Pose);
            m_openGLProgram->g_matProjection_0 = ComposeProjectionMatrix(viewProjections[0].Fov, viewProjections[0].NearFar);

            // transpose left eye matrix for shader usage.
            DirectX::XMStoreFloat4x4(&viewProjectionCBufferData.ViewProjection[0],
                                     DirectX::XMMatrixTranspose(m_openGLProgram->g_matView_0 * m_openGLProgram->g_matProjection_0));

            // Set view projection matrix for right view
            m_openGLProgram->g_matView_1 = xr::math::LoadInvertedXrPose(viewProjections[1].Pose);
            m_openGLProgram->g_matProjection_1 = ComposeProjectionMatrix(viewProjections[1].Fov, viewProjections[1].NearFar);

            // transpose right eye matrix for shader usage.
            DirectX::XMStoreFloat4x4(&viewProjectionCBufferData.ViewProjection[1],
                                     DirectX::XMMatrixTranspose(m_openGLProgram->g_matView_1 * m_openGLProgram->g_matProjection_1));

            m_deviceContext->UpdateSubresource(m_viewProjectionCBuffer.get(), 0, nullptr, &viewProjectionCBufferData, 0, 0);
            // Start  Rendering OpenGL
            //---------------------------------------------------------
            m_openGLProgram->render();
            //---------------------------------------------------------

            // Set DirectX square primitive data.
            const UINT strides[] = {sizeof(CubeShader::Vertex)};
            const UINT offsets[] = {0};
            ID3D11Buffer* vertexBuffers[] = {m_cubeVertexBuffer.get()};
            m_deviceContext->IASetVertexBuffers(0, (UINT)std::size(vertexBuffers), vertexBuffers, strides, offsets);
            m_deviceContext->IASetIndexBuffer(m_cubeIndexBuffer.get(), DXGI_FORMAT_R16_UINT, 0);
            m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            m_deviceContext->IASetInputLayout(m_inputLayout.get());

            // Render DirectX 
            m_deviceContext->DrawIndexedInstanced((UINT)std::size(CubeShader::c_cubeIndices), viewInstanceCount, 0, 0, 0);

        
          
        }
        void ClearView(ID3D11Texture2D* colorTexture, const float renderTargetClearColor[4]) override {
            winrt::com_ptr<ID3D11RenderTargetView> renderTargetView;
            CHECK_HRCMD(m_device->CreateRenderTargetView(colorTexture, nullptr, renderTargetView.put()));
            m_deviceContext->ClearRenderTargetView(renderTargetView.get(), renderTargetClearColor);


            samplerState = 0;
            textureView = 0;
        }
  
    public:
        winrt::com_ptr<ID3D11Device> m_device;
        winrt::com_ptr<ID3D11DeviceContext> m_deviceContext;

        winrt::com_ptr<ID3D11VertexShader> m_vertexShader;
        winrt::com_ptr<ID3D11PixelShader> m_pixelShader;
        winrt::com_ptr<ID3D11InputLayout> m_inputLayout;
        winrt::com_ptr<ID3D11Buffer> m_modelCBuffer;

        winrt::com_ptr<ID3D11Buffer> m_viewProjectionCBuffer;
        winrt::com_ptr<ID3D11Buffer> m_colorFilterCBuffer;
        winrt::com_ptr<ID3D11Buffer> m_cubeVertexBuffer;
        winrt::com_ptr<ID3D11Buffer> m_cubeIndexBuffer;
        winrt::com_ptr<ID3D11DepthStencilState> m_reversedZDepthNoStencilTest;

        ID3D11ShaderResourceView* textureView;
        ID3D11SamplerState* samplerState;
       
       OpenGLProgram* m_openGLProgram;

  };

} // namespace

namespace sample {
    std::unique_ptr<sample::IGraphicsPluginD3D11> CreateCubeGraphics() {
        auto _cubeGraphics = std::make_unique<CubeGraphics>();
        _cubeGraphics->m_openGLProgram = new OpenGLProgram();
        return _cubeGraphics;

    }


} // namespace sample



