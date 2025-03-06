// Copyright (c) 2025 Thomas Groom

#pragma once

#include <stdio.h>

#include "Utils.h"
#include "Button.h"


class TemplateWS : public WorkspaceComponent {
public:
	using ModelType = std::shared_ptr<TemplateModel>;
	TemplateWS(Boundary* mContainer, ModelType model);
	void Register() override;
	void Draw() override;

private:
	Boundary* mContainer;
	Primitive mPrim;
	ModelType model;
	Text TextRenderer;

	static std::map<std::string, Button*> Buttons;

};