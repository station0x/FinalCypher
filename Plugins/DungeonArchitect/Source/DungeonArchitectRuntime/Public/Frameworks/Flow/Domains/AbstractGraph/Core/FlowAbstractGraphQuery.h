//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Core/Utils/MathUtils.h"
#include "Frameworks/Flow/Domains/AbstractGraph/Core/FlowAbstractGraph.h"
#include "Frameworks/Flow/Domains/AbstractGraph/Core/FlowAbstractNode.h"

class UFlowAbstractGraphBase;
class UFlowAbstractLink;

class DUNGEONARCHITECTRUNTIME_API FFlowAbstractGraphTraversal {
public:
    struct FNodeInfo {
        FGuid NodeId;
        FGuid LinkId;
        bool bOutgoing = false;
    };

    void Build(UFlowAbstractGraphBase* InGraph);
    void GetOutgoingNodes(const FGuid& InNodeId, TArray<FNodeInfo>& OutResult) const;
    void GetIncomingNodes(const FGuid& InNodeId, TArray<FNodeInfo>& OutResult) const;
    void GetConnectedNodes(const FGuid& InNodeId, TArray<FNodeInfo>& OutResult) const;
    bool GetTeleportNode(const FGuid& InNodeId, FGuid& OutConnectedNodeId) const;
    
private:
    TMap<FGuid, TArray<FNodeInfo>> OutgoingNodes;
    TMap<FGuid, TArray<FNodeInfo>> IncomingNodes;
    TMap<FGuid, FGuid> Teleporters;        // Node -> Node mapping of teleporters
};

template<typename TNode>
class DUNGEONARCHITECTRUNTIME_API TFlowAbstractGraphQuery {
public:
    explicit TFlowAbstractGraphQuery(UFlowAbstractGraphBase* InGraph) {
        Graph = InGraph;
        Build();
    }
    
    TNode* GetNode(const FGuid& NodeId) const {
        const TWeakObjectPtr<UFlowAbstractNode>* SearchResult = NodeMap.Find(NodeId);
        if (!SearchResult) return nullptr;
        const TWeakObjectPtr<UFlowAbstractNode> NodePtr = *SearchResult;
        return Cast<TNode>(NodePtr.Get());
    }

    TNode* GetSubNode(const FGuid& NodeId) const {
        const TWeakObjectPtr<UFlowAbstractNode>* SearchResult = SubNodeMap.Find(NodeId);
        if (!SearchResult) return nullptr;
        const TWeakObjectPtr<UFlowAbstractNode> NodePtr = *SearchResult;
        return Cast<TNode>(NodePtr.Get());
    }
    
    FGuid GetNodeAtCoord(const FVector& InCoord) const {
        const FGuid* SearchResult = CoordToNodeMap.Find(InCoord);
        return SearchResult ? *SearchResult : FGuid();
    }
    
    TNode* GetNodeObjectAtCoord(const FIntVector& InCoord) const {
        return GetNodeObjectAtCoord(FMathUtils::ToVector(InCoord));
    }
    
    TNode* GetNodeObjectAtCoord(const FVector& InCoord) const {
        const FGuid* SearchResult = CoordToNodeMap.Find(InCoord);
        return SearchResult ? GetNode(*SearchResult) : nullptr;
    }
    
    UFlowAbstractLink* GetLink(const FGuid& NodeId) const {
        const TWeakObjectPtr<UFlowAbstractLink>* SearchResult = LinkMap.Find(NodeId);
        if (!SearchResult) return nullptr;
        const TWeakObjectPtr<UFlowAbstractLink> LinkPtr = *SearchResult;
        return LinkPtr.Get();
    };

    TArray<FGuid> GetOutgoingNodes(const FGuid& InNodeId) {
        TArray<FGuid>* SearchResult = OutgoingNodes.Find(InNodeId);
        return SearchResult ? *SearchResult : TArray<FGuid>();
    }
    
    TArray<FGuid> GetIncomingNodes(const FGuid& InNodeId) {
        TArray<FGuid>* SearchResult = IncomingNodes.Find(InNodeId);
        return SearchResult ? *SearchResult : TArray<FGuid>();
    }

    TArray<FGuid> GetConnectedNodes(const FGuid& InNodeId) {
        TArray<FGuid>* SearchResult = ConnectedNodes.Find(InNodeId);
        return SearchResult ? *SearchResult : TArray<FGuid>();
    }
    
    TArray<FGuid> GetConnectedLinks(const FGuid& InNodeId) {
        TArray<FGuid>* SearchResult = ConnectedLinks.Find(InNodeId);
        return SearchResult ? *SearchResult : TArray<FGuid>();
    }

    UFlowAbstractLink* GetConnectedLink(const FGuid& InNodeA, const FGuid& InNodeB) {
        TArray<FGuid> NodeAConnectedLinks = GetConnectedLinks(InNodeA);
        for (const FGuid& ConnectedLink : NodeAConnectedLinks) {
            UFlowAbstractLink* Link = GetLink(ConnectedLink);
            if (!Link) continue;
            if (Link->Source == InNodeB
                || Link->SourceSubNode == InNodeB
                || Link->Destination == InNodeB
                || Link->DestinationSubNode == InNodeB)
            {
                return Link;
            }
        }
        return nullptr;
    }

    bool GetParentNode(const FGuid& InNodeId, FGuid& OutParentNodeId) {
        FGuid* ParentIdPtr = ParentNodes.Find(InNodeId);
        if (!ParentIdPtr) {
            OutParentNodeId = FGuid();
            return false;
        }

        OutParentNodeId = *ParentIdPtr;
        return true;
    }

    void GetConnectedNodes(const FGuid& InNodeId, TArray<FGuid>& OutConnectedNodeIds, TArray<FGuid>& OutConnectedLinks) {
        OutConnectedNodeIds = GetConnectedNodes(InNodeId);
        OutConnectedLinks = GetConnectedLinks(InNodeId);
    }
    
    const FFlowAbstractGraphTraversal& GetTraversal() const { return Traversal; }
    
    template<typename T>
    T* GetGraph() const { return Cast<T>(Graph.Get()); }

    void Rebuild() { Build(); }
    
private:
    void Build() {
        NodeMap.Reset();
        LinkMap.Reset();
        OutgoingNodes.Reset();
        IncomingNodes.Reset();
        ParentNodes.Reset();
        ConnectedNodes.Reset();
        ConnectedLinks.Reset();
        SubNodeMap.Reset();
        CoordToNodeMap.Reset();
        
        for (UFlowAbstractNode* Node : Graph->GraphNodes) {
            NodeMap.Add(Node->NodeId, Node);
            FGuid& NodeIdRef = CoordToNodeMap.FindOrAdd(Node->Coord);
            NodeIdRef = Node->NodeId;
            for (UFlowAbstractNode* SubNode : Node->MergedCompositeNodes) {
                SubNodeMap.Add(SubNode->NodeId, SubNode);
                ParentNodes.Add(SubNode->NodeId, Node->NodeId);
                
                FGuid& SubNodeIdRef = CoordToNodeMap.FindOrAdd(SubNode->Coord);
                SubNodeIdRef = SubNode->NodeId;
            }
        }
        
        for (UFlowAbstractLink* Link : Graph->GraphLinks) {
            LinkMap.Add(Link->LinkId, Link);

            ConnectedNodes.FindOrAdd(Link->Source).Add(Link->Destination);
            ConnectedNodes.FindOrAdd(Link->Destination).Add(Link->Source);

            ConnectedLinks.FindOrAdd(Link->Source).Add(Link->LinkId);
            ConnectedLinks.FindOrAdd(Link->SourceSubNode).Add(Link->LinkId);
            ConnectedLinks.FindOrAdd(Link->Destination).Add(Link->LinkId);
            ConnectedLinks.FindOrAdd(Link->DestinationSubNode).Add(Link->LinkId);

            if (Link->Type != EFlowAbstractLinkType::Unconnected) {
                OutgoingNodes.FindOrAdd(Link->Source).Add(Link->Destination);
                IncomingNodes.FindOrAdd(Link->Destination).Add(Link->Source);
            }
        }
        
        Traversal.Build(Graph.Get());
    }
    
private:
    TMap<FGuid, TWeakObjectPtr<UFlowAbstractNode>> NodeMap;
    TMap<FGuid, TWeakObjectPtr<UFlowAbstractLink>> LinkMap;
    TMap<FGuid, TArray<FGuid>> OutgoingNodes;
    TMap<FGuid, TArray<FGuid>> IncomingNodes;
    TMap<FGuid, FGuid> ParentNodes;
    
    TMap<FGuid, TArray<FGuid>> ConnectedNodes;
    TMap<FGuid, TArray<FGuid>> ConnectedLinks;
    
    TMap<FGuid, TWeakObjectPtr<UFlowAbstractNode>> SubNodeMap;
    TMap<FVector, FGuid> CoordToNodeMap;
    
    TWeakObjectPtr<UFlowAbstractGraphBase> Graph;
    FFlowAbstractGraphTraversal Traversal;
    
};

typedef TFlowAbstractGraphQuery<UFlowAbstractNode> FFlowAbstractGraphQuery;

