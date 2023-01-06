#include "bvh.h"

namespace end
{
	bvh_node_t::bvh_node_t(bvh_node_t* root, uint32_t left_index, uint32_t right_index)
	{
		//TODO The root pointer is the array of the entire bvh
		//Build the aabb that encapsulates the left and right's aabb's
		/* TODO: Re-look*/
		_left = left_index;
		_right = right_index;
	}

	float bounding_volume_hierarchy_t::cost(const bvh_node_t& a, const bvh_node_t& b)
	{
		////TODO: calculate manhattan distance from the center points of the two aabb's
		//const Vector3D& CenterA = a.aabb().GetCenter();
		//const Vector3D& CenterB = b.aabb().GetCenter();
		//
		////float DeltaX = std::abs(CenterB.X - CenterA.X);
		////float DeltaY = std::abs(CenterB.Y - CenterA.Y);
		////float DeltaZ = std::abs(CenterB.Z - CenterA.Z);
		////
		////return DeltaX + DeltaY + DeltaZ;
		//return VrixicMath::ManhattanDistance(CenterA, CenterB);
		return 0.0f;
	}

	void bounding_volume_hierarchy_t::insert(const AABB& aabb, uint32_t element_id)
	{
		////TODO
		////create a bvh node using the passed in parameters(do not call new)
		//bvh_node_t Node(aabb , element_id);
		//Node.left() = 0;

		////TODO
		////if its the first node, it becomes the root. So just push it into bvh vector, then return
		//if (bvh.size() == 0)
		//{
		//	bvh.push_back(Node);
		//	return;
		//}

		//Vector3D NewNodeAABBMin = Node.aabb().GetCenter() - Node.aabb().GetExtent();
		//Vector3D NewNodeAABBMax = Node.aabb().GetCenter() + Node.aabb().GetExtent();
		//
		////TODO
		////start at index 0 of the bvh (root)
		////while the current node is a branch, 
		////Modify this node's aabb that encapsulates the current node, and the aabb that was passed in
		////figure out if you are going with the left or right child
		////change your index to be the one you have chosen.
		//uint32 LastNodeIndex = 0;
		//uint32 CurrentNodeIndex = 0;
		//while (bvh[CurrentNodeIndex].is_branch())
		//{
		//	LastNodeIndex = CurrentNodeIndex;

		//	{
		//		bvh[CurrentNodeIndex].aabb().CalculateCenterAndExtent(NewNodeAABBMin, NewNodeAABBMax);
		//	}

		//	/* Go left since left one is closer else go right since left one is further */
		//	CurrentNodeIndex = bvh[LastNodeIndex].right(); // assume its right until proven wrong (one less branch -> the else statement)
		//	if (cost(bvh[bvh[LastNodeIndex].left()], Node) < cost(bvh[bvh[LastNodeIndex].right()], Node))
		//	{
		//		CurrentNodeIndex = bvh[LastNodeIndex].left();
		//	}
		//}

		////TODO
		////Once you have left that while loop, you now hold onto a leaf node index where we will add the node to
		////the 2 new nodes' indices(left and right child) will be the newly created indices once you push back twice on the bvh vector
		////the current node (who is now a branch) needs to be sized to fit the two new nodes(left and right, remember don't need to call new).
		////set the parents of these two nodes
		//bvh_node_t LeftNode = bvh[CurrentNodeIndex]; /* Copy the leaf node and make it the left and the new Node will be right */
		//LeftNode.left() = 0;
		//LeftNode.parent() = CurrentNodeIndex;
		//Node.parent() = CurrentNodeIndex;

		//bvh.push_back(LeftNode);
		//bvh[CurrentNodeIndex].left() = bvh.size() - 1;

		//bvh.push_back(Node);
		//bvh[CurrentNodeIndex].right() = bvh.size() - 1;

		////Vector3D ParentExtent = LeftNode.aabb().GetExtent() + Node.aabb().GetExtent();
		////Vector3D ParentCenter = (LeftNode.aabb().GetCenter() + Node.aabb().GetCenter()) * 0.5f;

		//bvh[CurrentNodeIndex].aabb().CalculateCenterAndExtent(NewNodeAABBMin, NewNodeAABBMax);
	}
}