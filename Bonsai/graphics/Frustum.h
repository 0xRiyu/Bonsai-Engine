#pragma once
#include "../bonsai.h"
using namespace DirectX;


namespace bonsai
{
	namespace graphics
	{
		struct AABB;

		class Frustum
		{
		private:
			XMFLOAT4 m_PlanesNorm[6];
			XMFLOAT4 m_Planes[6];
			XMFLOAT4X4 view;

		public:
			Frustum();
			void CleanUp();

			void ConstructFrustum(float screenDepth, XMMATRIX projectionMat, const XMMATRIX& viewMatrix, const XMMATRIX& world = XMMatrixIdentity());


			bool CheckPoint(const XMFLOAT3& point) const;
			bool CheckSphere(const XMFLOAT3& center, float radius) const;
			bool CheckCube(float xCenter, float yCenter, float zCenter, float radius);

#define BOX_FRUSTUM_INTERSECTS 1
#define BOX_FRUSTUM_INSIDE 2
			int CheckBox(const AABB& box) const;

			const XMFLOAT4& getLeftPlane() const;
			const XMFLOAT4& getRightPlane() const;
			const XMFLOAT4& getTopPlane() const;
			const XMFLOAT4& getBottomPlane() const;
			const XMFLOAT4& getFarPlane() const;
			const XMFLOAT4& getNearPlane() const;
			XMFLOAT3 getCameraPos() const;

		};

		//TODO: Intersection Class
		struct AABB
		{
			enum INTERSECTION_TYPE
			{
				OUTSIDE,
				INTERSECTS,
				INSIDE,
			};

			XMFLOAT3 _min;
			XMFLOAT3 _max;
			AABB(const XMFLOAT3& _min = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX), const XMFLOAT3& _max = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX)) : _min(_min), _max(_max) {}
			
			AABB get(const XMMATRIX& mat) const
			{
				XMFLOAT3 corners[8];
				for (int i = 0; i < 8; ++i)
				{
					XMVECTOR point = XMVector3Transform(XMLoadFloat3(&corner(i)), mat);
					XMStoreFloat3(&corners[i], point);
				}
				XMFLOAT3 min = corners[0];
				XMFLOAT3 max = corners[6];

				for (int i = 0; i < 8; ++i)
				{
					const XMFLOAT3& p = corners[i];

					if (p.x < min.x) min.x = p.x;
					if (p.y < min.y) min.y = p.y;
					if (p.z < min.z) min.z = p.z;

					if (p.x > max.x) max.x = p.x;
					if (p.y > max.y) max.y = p.y;
					if (p.z > max.z) max.z = p.z;
				}

				return AABB(min, max);
			}

			inline XMFLOAT3 corner(int index) const
			{
				switch (index)
				{
				case 0: return _min;
				case 1: return XMFLOAT3(_min.x, _max.y, _min.z);
				case 2: return XMFLOAT3(_min.x, _max.y, _max.z);
				case 3: return XMFLOAT3(_min.x, _min.y, _max.z);
				case 4: return XMFLOAT3(_max.x, _min.y, _min.z);
				case 5: return XMFLOAT3(_max.x, _max.y, _min.z);
				case 6: return _max;
				case 7: return XMFLOAT3(_max.x, _min.y, _max.z);
				}
				assert(0);
				return XMFLOAT3(0, 0, 0);
			}
		};

	}
}