cbuffer CullingParams : register(b0) {
    uint numShapes;
    uint pad0;
    uint pad1;
    uint pad2;
    float4 bbMin[21];
    float4 bbMax[21];
};

cbuffer SceneData : register(b1) {
    float4x4 viewProjectionMatrix;
    float4 planes[6];
};

RWStructuredBuffer<uint> indirectArgsBuffer : register(u0);
RWStructuredBuffer<uint> objectsIdsBuffer : register(u1);

[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    uint idx = DTid.x;

    if (idx >= numShapes) {
        return;
    }

    float4 minVal = bbMin[idx];
    float4 maxVal = bbMax[idx];

    float3 corners[8];
    corners[0] = float3(minVal.x, minVal.y, minVal.z);
    corners[1] = float3(maxVal.x, minVal.y, minVal.z);
    corners[2] = float3(minVal.x, maxVal.y, minVal.z);
    corners[3] = float3(minVal.x, minVal.y, maxVal.z);
    corners[4] = float3(maxVal.x, maxVal.y, minVal.z);
    corners[5] = float3(maxVal.x, minVal.y, maxVal.z);
    corners[6] = float3(minVal.x, maxVal.y, maxVal.z);
    corners[7] = float3(maxVal.x, maxVal.y, maxVal.z);

    bool isVisible = true;
    for (int planeIdx = 0; planeIdx < 6; planeIdx++) {
        int outsideCount = 0;

        for (int cornerIdx = 0; cornerIdx < 8; cornerIdx++) {
            float distance = dot(planes[planeIdx].xyz, corners[cornerIdx]) + planes[planeIdx].w;
            if (distance < 0.0) {
                outsideCount++;
            }
        }

        if (outsideCount == 8) {
            isVisible = false;
            break;
        }
    }

    if (isVisible) {
        uint visibleIndex;
        InterlockedAdd(indirectArgsBuffer[1], 1, visibleIndex);
        objectsIdsBuffer[visibleIndex] = idx;
    }
}
