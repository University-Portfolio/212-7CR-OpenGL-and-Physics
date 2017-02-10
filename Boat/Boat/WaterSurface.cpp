#include "WaterSurface.h"
#include "Logger.h"
#include "ClipShader.h"
#include "Game.h"


WaterSurface::WaterSurface()
	: reflection_fbo(1024, 1024), refraction_fbo(1024, 1024)
{
	tags |= E_TAG_WATER;
	model_comp = MakeComponent<ModelComponent>();
	reflection_camera = MakeComponent<CameraComponent>();
	refraction_camera = MakeComponent<CameraComponent>();
}

void WaterSurface::WindowBegin() 
{
	__super::WindowBegin();

	model_comp->model = g_game->GetWindow()->GetModelLoader()["water_surface"];
	model_comp->shader = g_game->GetWindow()->GetShaderLoader()["water"];

	reflection_clip_shader = g_game->GetWindow()->GetShaderLoader()["reflection_clip"];
	refraction_clip_shader = g_game->GetWindow()->GetShaderLoader()["refraction_clip"];

	if ((ClipShader*)reflection_clip_shader)
		((ClipShader*)reflection_clip_shader)->plane = glm::vec4(0.0, 1.0, 0.0, 0.0);

	if ((ClipShader*)refraction_clip_shader)
		((ClipShader*)refraction_clip_shader)->plane = glm::vec4(0.0, -1.0, 0.0, 0.0);

	reflection_fbo.Create();
	refraction_fbo.Create();
	model_comp->SetTextureUnit(0, reflection_fbo.GetTextureID());
	model_comp->SetTextureUnit(1, refraction_fbo.GetTextureID());
}

void WaterSurface::WindowDestroy() 
{
	__super::WindowDestroy();
	reflection_fbo.CleanUp();
	refraction_fbo.CleanUp();
}

void WaterSurface::LogicTick(float delta_time)
{
	__super::LogicTick(delta_time);
	
}

void WaterSurface::WindowTick(float delta_time) 
{
	__super::WindowTick(delta_time);
	
	World* world = GetWorld();
	Renderer* renderer = world ? world->GetRenderer() : nullptr;

	if (!renderer)
		return;

	//Fix camera positions
	CameraComponent* main_camera = GetWorld()->GetMainCamera();

	if (main_camera)
	{
		reflection_camera->transform = main_camera->transform;
		refraction_camera->transform = main_camera->transform;

		reflection_camera->transform.location.y *= -1.0f;
		reflection_camera->transform.previous_location.y *= -1.0f;
		reflection_camera->transform.rotation.x *= -1.0f;
		reflection_camera->transform.previous_rotation.x *= -1.0f;
	}

	const float aspect_ratio = g_game->GetWindow()->GetAspectRatio();

	//Render reflection
	{
		RenderSettings render_settings;
		render_settings.aspect_ratio = aspect_ratio;
		render_settings.camera = reflection_camera;
		render_settings.frame_buffer = &reflection_fbo;
		render_settings.blacklist = E_TAG_WATER;

		render_settings.shader_override = reflection_clip_shader;
		render_settings.shader_override_tags = E_TAG_ALL & ~E_TAG_SKYBOX;

		renderer->AddRenderTarget(render_settings);
	}

	//Render refraction
	{
		RenderSettings render_settings;
		render_settings.aspect_ratio = aspect_ratio;
		render_settings.camera = refraction_camera;
		render_settings.frame_buffer = &refraction_fbo;
		render_settings.blacklist = E_TAG_WATER;

		render_settings.shader_override = refraction_clip_shader;
		render_settings.shader_override_tags = E_TAG_ALL & ~E_TAG_SKYBOX;

		renderer->AddRenderTarget(render_settings);
	}
}