#include <cstdint>
#include <cassert>
#include "Renderer/Renderer/Box.h"
#include <vector>
#include <functional>
#include <unordered_map>
#include "Renderer/Renderer/Model.h"

namespace end
{
	// bvh_node_t
	//
	// BVH will be stored as a vector/array of this type
	// A node will always be at a higher index/address than its parent
	class bvh_node_t
	{
	public:
		// TODO: This constructor is the only function for you to implement in this file.
		bvh_node_t(bvh_node_t* root, uint32_t left_index, uint32_t right_index);

		bvh_node_t(AABB* bounds, uint32_t id) : _aabb{ bounds }, _id{ id } { }

		bvh_node_t() = default;
		bvh_node_t(const bvh_node_t&) = default;

		~bvh_node_t()
		{
			delete _aabb;
		}

		// Root must be at [0], so parent index must be invalid
		inline bool is_root()const { return _parent == UINT32_MAX; }

		inline bool is_branch()const { return _is_branch; }

		inline bool is_leaf()const { return !is_branch(); }

		inline uint32_t element_id()const
		{
			//assert(is_leaf());
			return _id;
		}

		inline uint32_t& left()
		{
			//assert(is_branch());
			return _left;
		}

		inline uint32_t left()const
		{
			//assert(is_branch());
			return _left;
		}

		inline uint32_t& right()
		{
			//assert(is_branch());
			return _right;
		}

		inline uint32_t right()const
		{
			assert(is_branch());
			return _right;
		}

		inline uint32_t parent()const
		{
			assert(!is_root());
			return _parent;
		}

		uint32_t& parent()
		{
			//assert(!is_root());
			return _parent;
		}

		AABB* aabb()
		{
			return _aabb;
		}

		const AABB* aabb()const
		{
			return _aabb;
		}

	private:

		// Indices for connected nodes
		uint32_t _parent = UINT32_MAX;

		// Index for left child.
		// Branches must have two children.
		// If "this" is a leaf, _left/_is_branch will be 0.
		union
		{
			uint32_t _left;
			uint32_t _is_branch;
		};

		// Index for right child.
		// Branches must have two children.
		// If "this" is a leaf, _right/_id stores the element id.
		union
		{
			uint32_t _right;
			uint32_t _id;
		};

		// AABB for this node
		AABB* _aabb;
	};

	struct bounding_volume_hierarchy_t
	{
		~bounding_volume_hierarchy_t()
		{
			for (uint32 i = 0; i < bvh.size(); ++i)
			{
				delete bvh[i];
			}
		}
#define VERTS_PER_AABB 3
		/* Set the box indices once */
		uint32 BoxIndices[24] =
		{
			0, 1, 1, 2, 2, 3, 3, 0,
			4, 5, 5, 6, 6, 7, 7, 4,
			0, 4, 1, 5, 2, 6, 3, 7
		};

		std::vector<bvh_node_t*> bvh;

		inline bvh_node_t* node_at(uint32_t i) { return bvh[i]; }

		inline size_t node_count()const { return bvh.size(); }

		static float cost(const bvh_node_t& a, const bvh_node_t& b);

		//TODO
		//You will need to create a traversal function to render the aabb's as they collide on the way down
		//You could create a callback function as well and that function does the collision tests and drawws the aabb lines
		//a recursive depth-first function could work
		template<class T>
		void CheckCollision(uint32 currentNode, const AABB& tester, T& const t, void(T::* collisionCallback)(const AABB& collider))
		{
			if (AABB::IntersectAABBToAABB(*node_at(currentNode)->aabb(), tester) == IntersectionResult::Intersection)
			{
				if (node_at(currentNode)->is_leaf())
				{
					(t.*collisionCallback)(*node_at(currentNode)->aabb()); /* Triggers the callback */
					//node_at(currentNode).aabb().DrawBox(end::float4(1, 0, 1, 1), BoxIndices); /* This nodes aabb is colliding */
					return;
				}

				node_at(currentNode)->aabb()->DrawBox(end::float4(1, 0, 0, 1), BoxIndices);

				CheckCollision(node_at(currentNode)->left(), tester, t, collisionCallback);
				CheckCollision(node_at(currentNode)->right(), tester, t, collisionCallback);
			}

			return;
		}

		// Add an aabb/element_id pair to the bvh
		void insert(const AABB& aabb, uint32_t element_id);

		uint32 TopDownTreeCreation(const std::vector<TerrainVertex>& inVertices, std::vector<uint32> inIndices,
			uint32 inParentNode = UINT32_MAX)
		{
			static uint32 ParentOffset = 0;

			if (inIndices.size() <= VERTS_PER_AABB)
			{
				return 0; /* No More Cutting in half */
			}

			Vector3D Min = Vector3D(FLT_MAX, FLT_MAX, FLT_MAX);
			Vector3D Max = Vector3D(FLT_MIN, FLT_MIN, FLT_MIN);

			//float MinX = FLT_MAX;
			//float MinY = FLT_MAX;
			//float MinZ = FLT_MAX;

			//float MaxX = FLT_MIN;
			//float MaxY = FLT_MIN;
			//float MaxZ = FLT_MIN;

			std::vector<uint32> InFrontIndices;
			std::vector<uint32> BehindIndices;

			Vector3D CenterOfMass = Vector3D::ZeroVector();

			for (uint32 i = 0; i < inIndices.size(); ++i)
			{
				Vector3D Position = inVertices[inIndices[i]].Position;

				if (Position.X < Min.X)
				{
					Min.X = Position.X;
				}
				if (Position.Y < Min.Y)
				{
					Min.Y = Position.Y;
				}
				if (Position.Z < Min.Z)
				{
					Min.Z = Position.Z;
				}

				if (Position.X > Max.X)
				{
					Max.X = Position.X;
				}
				if (Position.Y > Max.Y)
				{
					Max.Y = Position.Y;
				}
				if (Position.Z > Max.Z)
				{
					Max.Z = Position.Z;
				}

				/* Last three verts =  triangle */
				CenterOfMass += inVertices[inIndices[i]].Position;
			}

			CenterOfMass /= inIndices.size();

			Vector3D CenterOfMassTranslated = CenterOfMass;

			float DeltaX = Max.X - Min.X;
			float DeltaZ = Max.Z - Min.Z;
			if (DeltaX > DeltaZ)
			{
				CenterOfMassTranslated.X += 1.0f;
			}
			else
			{
				CenterOfMassTranslated.Z += 1.0f;
			}

			Vector3D ToCenterOfMassTranslated = (CenterOfMassTranslated - CenterOfMass).Normalize();

			/* Split vertices into two groups -> infront or behind center of mass */
			for (uint32 i = 0; i < inIndices.size(); i += 3)
			{
				Vector3D TriangleCenterOfMass = (inVertices[inIndices[i]].Position
					+ inVertices[inIndices[i + 1]].Position
					+ inVertices[inIndices[i + 2]].Position) / 3.0f;

				Vector3D ToTriangleCenterOfMass = (TriangleCenterOfMass - CenterOfMass).Normalize();

				if (Vector3D::DotProduct(ToCenterOfMassTranslated, ToTriangleCenterOfMass) < 0.0f) // Behind 
				{
					BehindIndices.push_back(inIndices[i]);
					BehindIndices.push_back(inIndices[i + 1]);
					BehindIndices.push_back(inIndices[i + 2]);

					continue;
				}

				InFrontIndices.push_back(inIndices[i]);
				InFrontIndices.push_back(inIndices[i + 1]);
				InFrontIndices.push_back(inIndices[i + 2]);
			}

			if ((BehindIndices.size() == VERTS_PER_AABB && InFrontIndices.size() != VERTS_PER_AABB)
				|| (BehindIndices.size() != VERTS_PER_AABB && InFrontIndices.size() == VERTS_PER_AABB)) /* Un even indices , so to even it out borrow */
			{
				if (BehindIndices.size() < InFrontIndices.size())
				{
					BehindIndices.push_back(InFrontIndices[0]);
					BehindIndices.push_back(InFrontIndices[1]);
					BehindIndices.push_back(InFrontIndices[2]);
				}
				else
				{
					InFrontIndices.push_back(BehindIndices[0]);
					InFrontIndices.push_back(BehindIndices[1]);
					InFrontIndices.push_back(BehindIndices[2]);
				}
			}

			AABB* Box = new AABB(Min, Max);
			bvh_node_t* Node = new bvh_node_t(Box, bvh.size());
			Node->parent() = inParentNode;

			uint32 Size = bvh.size();

			bvh.push_back(Node);

			uint32 LeftIndex = TopDownTreeCreation(inVertices, InFrontIndices, Node->element_id());
			if (LeftIndex == 0)
			{
				Node->left() = 0;
				ParentOffset++;
				return inParentNode + ParentOffset;
			}

			Node->left() = LeftIndex;
			Node->right() = TopDownTreeCreation(inVertices, BehindIndices, Node->element_id());

			ParentOffset = 0;
			return Size;
		}

		uint32 TopDownTreeCreation2(const std::vector<TerrainVertex>& inVertices, std::vector<uint32> inIndices,
			std::vector<uint32>& out, uint32 inParentNode = UINT32_MAX)
		{
			out.clear();
			if (inIndices.size() <= VERTS_PER_AABB)
			{
				return 0; /* No More Cutting in half */
			}

			float MinX = FLT_MAX;
			float MinY = FLT_MAX;
			float MinZ = FLT_MAX;

			float MaxX = FLT_MIN;
			float MaxY = FLT_MIN;
			float MaxZ = FLT_MIN;

			std::vector<uint32> InFrontIndices;
			std::vector<uint32> BehindIndices;

			Vector3D CenterOfMass = Vector3D::ZeroVector();

			uint32 XChangeCount = 0;
			uint32 ZChangeCount = 0;

			for (uint32 i = 0; i < inIndices.size(); ++i)
			{
				Vector3D Position = inVertices[inIndices[i]].Position;

				if (Position.X < MinX)
				{
					MinX = Position.X;
				}
				if (Position.Y < MinY)
				{
					MinY = Position.Y;
				}
				if (Position.Z < MinZ)
				{
					MinZ = Position.Z;
				}

				if (Position.X > MaxX)
				{
					MaxX = Position.X;
					XChangeCount++;
				}
				if (Position.Y > MaxY)
				{
					MaxY = Position.Y;
				}
				if (Position.Z > MaxZ)
				{
					MaxZ = Position.Z;
					ZChangeCount++;
				}

				/* Last three verts =  triangle */
				CenterOfMass += inVertices[inIndices[i]].Position;
			}

			CenterOfMass /= inIndices.size();

			Vector3D CenterOfMassTranslated = CenterOfMass;
			if (XChangeCount > ZChangeCount)
			{
				CenterOfMassTranslated.X += 1.0f;
			}
			else
			{
				CenterOfMassTranslated.Z += 1.0f;
			}

			Vector3D ToCenterOfMassTranslated = (CenterOfMassTranslated - CenterOfMass).Normalize();

			/* Split vertices into two groups -> infront or behind center of mass */
			for (uint32 i = 0; i < inIndices.size(); i += 3)
			{
				Vector3D TriangleCenterOfMass = (inVertices[inIndices[i]].Position
					+ inVertices[inIndices[i + 1]].Position
					+ inVertices[inIndices[i + 2]].Position) / 3.0f;

				Vector3D ToTriangleCenterOfMass = (TriangleCenterOfMass - CenterOfMass).Normalize();

				//if (XChangeCount > ZChangeCount)
				{
					if (Vector3D::DotProduct(ToCenterOfMassTranslated, ToTriangleCenterOfMass) < 0.0f) // Behind 
					{
						out.push_back(inIndices[i]);
						out.push_back(inIndices[i + 1]);
						out.push_back(inIndices[i + 2]);

						continue;
					}

					InFrontIndices.push_back(inIndices[i]);
					InFrontIndices.push_back(inIndices[i + 1]);
					InFrontIndices.push_back(inIndices[i + 2]);
				}
				//else
				//{
				//	if (Vector3D::DotProduct(ToCenterOfMassTranslated, ToTriangleCenterOfMass) < 0.0f) // Behind 
				//	{
				//		InFrontIndices.push_back(inIndices[i]);
				//		InFrontIndices.push_back(inIndices[i + 1]);
				//		InFrontIndices.push_back(inIndices[i + 2]);

				//		continue;
				//	}

				//	out.push_back(inIndices[i]);
				//	out.push_back(inIndices[i + 1]);
				//	out.push_back(inIndices[i + 2]);
				//}

			}
		}
	};

	// Declares a short-hand alias
	using bvh_t = bounding_volume_hierarchy_t;
}