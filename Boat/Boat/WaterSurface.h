#pragma once
#include "Entity.h"
#include "ModelComponent.h"

class WaterSurface : public Entity
{
private:
	ModelComponent* model_comp;

public:
	WaterSurface();


protected:
	//virtual void LogicBegin() override;
	virtual void WindowBegin() override;

	//virtual void LogicTick(float delta_time) override;
	//virtual void WindowTick(float delta_time) override;

	//virtual void LogicDestroy() override;
	//virtual void WindowDestroy() override;
};

