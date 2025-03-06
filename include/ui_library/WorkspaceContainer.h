// Copyright (c) 2025 Thomas Groom

#pragma once

#include "Utils.h"
#include <GLFW/glfw3.h>
#include <functional>
#include <memory>
#include <typeindex>
#include <any>
#include <unordered_map>
#include <string>

#include "Button.h"
#include "DropdownButton.h"
#include "ui_library/Config.h"

// A constant offset for the Y-coordinate in layout.
const int yOffset = 30;

struct Bounds {
	double W = 0.0f;  // startX
	double E = 1.0f;  // endX
	double N = 0.0f;  // startY
	double S = 1.0f;  // endY
};

class WorkspaceContainer
{
public:
    // The generic model registry.
    using ModelMap = std::unordered_map<std::string, std::any>;
    // A workspace factory function creates a WorkspaceComponent given a Boundary pointer and the models.
    using WorkspaceFactory = std::function<std::unique_ptr<WorkspaceComponent>(UI* _ui, Boundary*, const ModelMap&)>;

    // Structure that holds registration info.
    struct WorkspaceRegistration {
        int type;
        std::string name;
        WorkspaceFactory factory;
        // A prototype button for this workspace.
        std::shared_ptr<Button> buttonPrototype;
    };

    // Register a workspace type along with a name. This function creates a prototype button
    // (using default parameters) and adds it to the static Buttons map.
    static void RegisterWorkspace(UI* _ui, int type, const std::string& name, WorkspaceFactory factory);

    // Access to the static registrations.
    static const std::unordered_map<int, WorkspaceRegistration>& GetWorkspaceRegistrations();

    // Access to the static Buttons map.
    static const std::map<std::string, std::shared_ptr<Button>>& GetButtons();

    // The constructor now takes a generic model map.
    WorkspaceContainer(UI* _ui, Bounds _b = Bounds(), const ModelMap& models = ModelMap());
    ~WorkspaceContainer();

    // Sets the current workspace using a registered factory.
    void setWorkspace(int workspaceType);
    // Splits the container (horizontally or vertically) into two child containers.
    void split(bool _type, double _split);
    // Recursively draws this container (or its children) to the screen.
    void DrawContainers(WorkspaceContainer& node);

    // Public members for convenience.
    GLFWwindow* mWindow;
    std::unique_ptr<WorkspaceContainer> A;
    std::unique_ptr<WorkspaceContainer> B;

private:
    UI* mUI;
    // Draws this container into a specified pixel region.
    void Draw(int x, int y, int w, int h);
    // Used to constrain split resizing: recursively calculates bounds.
    Bounds recursiveMinMaxSearch(const WorkspaceContainer& node);

    // The generic model container used by workspaces.
    ModelMap models;
    // The actual workspace (leaf) – a subclass of WorkspaceComponent.
    std::unique_ptr<WorkspaceComponent> LeafWorkspace;

    // A selector button for choosing the workspace type.
    DropdownButton* WS_Selector_Button;
    std::shared_ptr<Texture2D> fileNewIcon;
    // The container boundary used by the contained workspace.
    Boundary mContainer = Boundary(0, 0, 10, 10);
    // The layout bounds (in relative coordinates).
    Bounds boundingBox;

    int mWorkspaceType;
    bool type;          // true = horizontal split, false = vertical split.
    double splitRatio;  // Split ratio (0.0 to 1.0).
    bool isLeaf;
    bool isResizing;

    Primitive mPrim;
    std::shared_ptr<Text> UIText = std::make_shared<Text>(std::string(UI_LIBRARY_RESOURCES_DIR) + "/fonts/arial.ttf", 12);

    // Static containers for workspace registrations and prototype buttons.
    static std::unordered_map<int, WorkspaceRegistration> sWorkspaceRegistrations;
    static std::map<std::string, std::shared_ptr<Button>> Buttons;
};


/* TODO NEXT:
Actually all of the view classes like FileBrowserWS inherit from WorkspaceComponent so maybe make that a requirenment in this class.


class WorkspaceComponent {
public:
	WorkspaceComponent() {}
    virtual ~WorkspaceComponent() {}

    // Common interface method for all components
    //virtual void Register() = 0;
    virtual void Draw() = 0;

};
*/