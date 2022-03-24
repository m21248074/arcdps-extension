#include "DemoPositioningComponent.h"

Position& DemoPositioningComponent::getPositionMode() {
	return mPositionMode;
}

CornerPosition& DemoPositioningComponent::getCornerPosition() {
	return mCornerPosition;
}

ImVec2& DemoPositioningComponent::getCornerVector() {
	return mCornerVector;
}

CornerPosition& DemoPositioningComponent::getAnchorPanelCorner() {
	return mAnchorPanelCornerPosition;
}

CornerPosition& DemoPositioningComponent::getSelfPanelCorner() {
	return mSelfPanelCornerPosition;
}

ImGuiID& DemoPositioningComponent::getFromWindowId() {
	return mFromWindowID;
}