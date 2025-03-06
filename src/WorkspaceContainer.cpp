// Copyright (c) 2025 Thomas Groom


#include "ui_library/WorkspaceContainer.h"

/*
	[ ] TODO: Switching between button sprites for different workspaces

    USEAGE:
    WorkspaceContainer::RegisterWorkspace(0, [](Boundary* b, const WorkspaceContainer::ModelMap& models) -> std::unique_ptr<WorkspaceComponent> {
        auto scene = std::any_cast<std::shared_ptr<Scene>>(models.at("Scene"));
        return std::make_unique<ViewportWS>(b, scene);
    });
*/


// ---------------------------------------------------------------------------
// Static members
// ---------------------------------------------------------------------------
std::unordered_map<int, WorkspaceContainer::WorkspaceRegistration> WorkspaceContainer::sWorkspaceRegistrations;
std::map<std::string, std::shared_ptr<Button>> WorkspaceContainer::Buttons;

// ---------------------------------------------------------------------------
// Static registration functions
// ---------------------------------------------------------------------------
void WorkspaceContainer::RegisterWorkspace(UI* _ui, int type, const std::string& name, WorkspaceFactory factory) {
    // Create a prototype button using default parameters.
    // (Note: In a more robust solution, you might parameterize font, boundary, alignment, etc.)
    std::shared_ptr<Button> prototype = std::make_shared<Button>(
        _ui,
        std::make_shared<Text>(std::string(UI_LIBRARY_RESOURCES_DIR) + "/fonts/arial.ttf", 12),
        std::wstring(name.begin(), name.end()),
        Text::LEFT_MIDDLE,
        Boundary(0, 0, 140, 20),
        5, 0.51f, 5
    );

    WorkspaceRegistration reg = { type, name, factory, prototype };
    sWorkspaceRegistrations[type] = reg;
    Buttons[name] = prototype;
}

const std::unordered_map<int, WorkspaceContainer::WorkspaceRegistration>& WorkspaceContainer::GetWorkspaceRegistrations() {
    return sWorkspaceRegistrations;
}

const std::map<std::string, std::shared_ptr<Button>>& WorkspaceContainer::GetButtons() {
    return Buttons;
}



// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------
WorkspaceContainer::WorkspaceContainer(UI* _ui, Bounds _b, const ModelMap& models)
    : mUI(_ui), models(models), boundingBox(_b), mWindow(nullptr),
      isLeaf(true), isResizing(false), splitRatio(0.5), type(true), // default: horizontal split
      mContainer(0, 0, 1, 1)
{
    // Build a list of buttons for the dropdown from the registered workspace prototypes.
    std::vector<std::shared_ptr<Button>> childButtons;
    for (const auto& entry : sWorkspaceRegistrations) {
        // Create a new button for this container instance.
        // (If your Button class supports cloning, you could clone the prototype instead.)
        std::shared_ptr<Button> btn = std::make_shared<Button>(
            mUI,
            UIText,
            std::wstring(entry.second.name.begin(), entry.second.name.end()),
            Text::LEFT_MIDDLE,
            Boundary(0, 0, 140, 20),
            5, 0.51f, 5
        );
        // Bind the callback to call this container’s setWorkspace.
        int workspaceType = entry.second.type;
        btn->SetOnClickCallback([this, workspaceType]() {
            this->setWorkspace(workspaceType);
        });
        childButtons.push_back(btn);
    }

    WS_Selector_Button = new DropdownButton(mUI, UIText, L"", Text::CENTER_MIDDLE, {0, 0, 30, 20}, 5, 0.7f);
    fileNewIcon = std::make_shared<Texture2D>(GL_RGBA, GL_RGBA, std::filesystem::path(std::string(UI_LIBRARY_RESOURCES_DIR) + "/icons/file_new.png"), glm::vec2(20));
    WS_Selector_Button->setIcon(fileNewIcon);
    WS_Selector_Button->SetChildButtons(childButtons);
    WS_Selector_Button->SetChildWidth(140);
}


WorkspaceContainer::~WorkspaceContainer() {
    A.reset();
    B.reset();
    LeafWorkspace.reset();
    delete WS_Selector_Button;
}


// ---------------------------------------------------------------------------
// Workspace selection and splitting
// ---------------------------------------------------------------------------
void WorkspaceContainer::setWorkspace(int workspaceType) {
    mWorkspaceType = workspaceType;
    // Look up the workspace registration by type.
    auto it = sWorkspaceRegistrations.find(workspaceType);
    if(it != sWorkspaceRegistrations.end()) {
        // Create the workspace using the registered factory.
        LeafWorkspace = it->second.factory(mUI, &mContainer, models);
    } else {
        printf("Invalid workspace type.");
        throw std::invalid_argument("Invalid workspace type.");
    }
}

void WorkspaceContainer::split(bool _type, double _split) {
    type = _type;
    splitRatio = _split;
    if (LeafWorkspace) LeafWorkspace.reset(nullptr);
    isLeaf = false;
    
    // Calculate new bounds for child containers.
    Bounds a = boundingBox, b = boundingBox;
    if (_type == true) { // Horizontal split
        a.E = boundingBox.W + _split * (boundingBox.E - boundingBox.W);
        b.W = a.E;
    } else {             // Vertical split
        a.S = boundingBox.N + _split * (boundingBox.S - boundingBox.N);
        b.N = a.S;
    }
    
    // Create child containers, passing the same model registry.
    A = std::make_unique<WorkspaceContainer>(mUI, a, models);
    B = std::make_unique<WorkspaceContainer>(mUI, b, models);

    // Optionally, set the same workspace type for the children.
    A->setWorkspace(mWorkspaceType);
    B->setWorkspace(mWorkspaceType);
}

// ---------------------------------------------------------------------------
// Drawing functions
// ---------------------------------------------------------------------------
Bounds WorkspaceContainer::recursiveMinMaxSearch(const WorkspaceContainer& node) {
    if (node.isLeaf) return node.boundingBox;

    Bounds a = recursiveMinMaxSearch(*node.A);
    Bounds b = recursiveMinMaxSearch(*node.B);

    return {
        glm::max(a.W, b.W),  // Largest startX
        glm::min(a.E, b.E),  // Smallest endX
        glm::max(a.N, b.N),  // Largest startY
        glm::min(a.S, b.S)   // Smallest endY
    };
}

void WorkspaceContainer::Draw(int x, int y, int xEnd, int yEnd) {
    mContainer.width = (xEnd - x) - 4;
    mContainer.height = (yEnd - y) - 4;
    mContainer.x = x + 2;
    mContainer.y = y + 2;
    
    glScissor(mContainer.x, mUI->G_HEIGHT - (mContainer.y + mContainer.height), mContainer.width, mContainer.height);

    if (LeafWorkspace != nullptr)
        LeafWorkspace->Draw();
    
    #if !BRAX_EDITOR_ONLY
        WS_Selector_Button->SetPos(mContainer.x + 3, mContainer.y + 3);
        WS_Selector_Button->Draw();
    #endif

    glScissor(0, 0, mUI->G_WIDTH, mUI->G_HEIGHT);
}

void WorkspaceContainer::DrawContainers(WorkspaceContainer& node) {
    if (&node == nullptr) return;

    bool isVert = node.type;

    int scaleW = mUI->G_WIDTH;
    int scaleH = (mUI->G_HEIGHT - 19) - yOffset;
    int Ax = (node.boundingBox.W * scaleW);
    int Ay = (node.boundingBox.N * scaleH) + yOffset;
    int endX = (node.boundingBox.E * scaleW);
    int endY = (node.boundingBox.S * scaleH) + yOffset;

    if (node.isLeaf) {
        node.Draw(Ax, Ay, endX, endY);
        return;
    }

    int splitX = node.splitRatio * scaleW;
    int splitY = node.splitRatio * scaleH + yOffset;

    int dragMargin = 4;
    bool isMouseNearSplit = (isVert &&
                             mUI->G_MOUSE_X >= splitX - dragMargin && mUI->G_MOUSE_X <= splitX + dragMargin &&
                             mUI->G_MOUSE_Y > Ay + dragMargin && mUI->G_MOUSE_Y < endY - dragMargin)
                         || (!isVert &&
                             mUI->G_MOUSE_X > Ax + dragMargin && mUI->G_MOUSE_X < endX - dragMargin &&
                             mUI->G_MOUSE_Y >= splitY - dragMargin && mUI->G_MOUSE_Y <= splitY + dragMargin);

    if (isMouseNearSplit) {
        mUI->G_SET_CURSOR = isVert ? CURSOR_HRESIZE : CURSOR_VRESIZE;
        if (mUI->G_LEFT_MOUSE_STATE == GLFW_PRESS)
            node.isResizing = true;
    }
    
    if (mUI->G_LEFT_MOUSE_STATE == GLFW_RELEASE)
        node.isResizing = false;

    if (node.isResizing) {
        mUI->G_SET_CURSOR = isVert ? CURSOR_HRESIZE : CURSOR_VRESIZE;
        node.splitRatio = isVert ? mUI->G_MOUSE_X / scaleW : (mUI->G_MOUSE_Y - yOffset) / scaleH;
    }

    if (mUI->G_RESIZE_FLAG || node.isResizing) {
        Bounds a = recursiveMinMaxSearch(*node.A);
        Bounds b = recursiveMinMaxSearch(*node.B);
        if (isVert)
            node.splitRatio = glm::clamp(node.splitRatio, a.W + 30.0f / scaleW, b.E - 30.0f / scaleW);
        else
            node.splitRatio = glm::clamp(node.splitRatio, a.N + 30.0f / scaleH, b.S - 30.0f / scaleH);
    }

    node.A->boundingBox = {node.boundingBox.W, isVert ? node.splitRatio : node.boundingBox.E,
                           node.boundingBox.N, isVert ? node.boundingBox.S : node.splitRatio};
    node.B->boundingBox = {isVert ? node.splitRatio : node.boundingBox.W, node.boundingBox.E,
                           isVert ? node.boundingBox.N : node.splitRatio, node.boundingBox.S};

    node.DrawContainers(*node.A);
    node.DrawContainers(*node.B);
}


//void WorkspaceContainer::deleteTree(WorkspaceContainer* node) {
//	if (!node) return;
//	deleteTree(node->A);
//	deleteTree(node->B);
//	delete node;
//}
/*
if (G_RESIZE_FLAG || node.isResizing) {
		Bounds a = recursiveMinMaxSearch(*node.A); //TODO: This should only need to be done once per drag
		Bounds b = recursiveMinMaxSearch(*node.B);
		*/
	//mPrim.RoundedRect(mContainer.x, mContainer.y, mContainer.width, mContainer.height, 0); //- 59
	//mPrim.SetColour(Colour(1.0f, 0.0f, 1.0f));
	//mPrim.Draw();