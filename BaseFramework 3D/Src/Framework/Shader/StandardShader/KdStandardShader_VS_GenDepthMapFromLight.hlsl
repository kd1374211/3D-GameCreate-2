#include "inc_KdStandardShader.hlsli"
#include "../inc_KdCommon.hlsli"

//================================
// 頂点シェーダ：影深度
//================================
VSOutputGenShadow main(
	float4 pos : POSITION,		// 頂点座標
	float2 uv : TEXCOORD0,		// テクスチャUV座標
	float4 color : COLOR,		// 頂点カラー
	float3 normal : NORMAL,		// 法線
	float3 tangent : TANGENT)	// 接線
{
	VSOutputGenShadow Out;
	
	// キャラクターの座標変換 : ローカル座標系 -> ワールド座標系へ変換
	Out.Pos = mul(pos, g_mWorld);
	
	// カメラの逆向きに変換 : ワールド座標系 -> ビュー座標系 -> 射影座標系へ変換
	Out.Pos = mul(Out.Pos, g_DL_mLightVP);

	// 射影行列を変換されないように保存
	Out.pPos = Out.Pos;
	
	// UV座標
	Out.UV = uv;

	Out.Color = color;

	// 出力
	return Out;
}
