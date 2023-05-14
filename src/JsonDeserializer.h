#pragma once

#include <fstream>

#include "Vector3.h"
#include "Material.h"
#include "Sphere.h"
#include "MovingSphere.h"
#include "Rectangle.h"
#include "Box.h"
#include "Instance.h"
#include "Volume.h"
#include "Camera.h"
#include "Scene.h"

#include "nlohmann/json.hpp"
using json = nlohmann::ordered_json;


// Forward declaration of JSON deserialization functions
void from_json(const json&, Vector3&);
void from_json(const json&, Camera&);
void from_json(const json&, std::shared_ptr<Texture>&);
void from_json(const json&, std::shared_ptr<Material>&);
void from_json(const json&, std::shared_ptr<Hittable>&);
void from_json(const json&, Scene&);


class JsonDeserializer
{
public:

    static Scene LoadScene(const std::string& filename)
    {
        std::ifstream file(filename);
        json json_data;
        file >> json_data;
        file.close();

        return json_data.get<Scene>();
    }
};


// Vector3 deserialization
void from_json(const json& j, Vector3& vec)
{
    j.get_to(vec.values);
}


// Camera deserialization
void from_json(const json& j, Camera& c)
{
    c = Camera(
        j.at("position").get<Point3>(),
        j.at("lookAt").get<Point3>(),
        j.at("worldUp").get<Vector3>(),
        j.at("verticalFov").get<double>(),
        j.at("aperture").get<double>(),
        j.at("focusDistance").get<double>(),
        j.at("timeShutterOpen").get<double>(),
        j.at("timeShutterClose").get<double>()
    );
}


// Texture deserialization
void from_json(const json& j, std::shared_ptr<Texture>& texture)
{
    const std::string type = j.at("type").get<std::string>();

    if (type == "SolidColor")
        texture = std::make_shared<SolidTexture>(j.at("color").get<Color>());
    else if (type == "Checkerboard")
        texture = std::make_shared<CheckerTexture>(j.at("even").get<Color>(), j.at("odd").get<Color>(), j.at("scale").get<double>());
    else if (type == "Noise")
        texture = std::make_shared<NoiseTexture>(j.at("color").get<Color>(), j.at("scale").get<double>());
    else if (type == "Marble")
        texture = std::make_shared<MarbleTexture>(j.at("color").get<Color>(), j.at("scale").get<double>(), j.at("turbulence").get<double>());
    else if (type == "Image")
        texture = std::make_shared<ImageTexture>(j.at("filename").get<std::string>());
    else throw std::exception(("Invalid texture type: " + type).c_str());
}


// Material deserialization
void from_json(const json& j, std::shared_ptr<Material>& material)
{
	const std::string type = j.at("type").get<std::string>();

	if (type == "LambertianColor")
        material = std::make_shared<LambertianColor>(j.at("albedo").get<Color>());
    else if (type == "LambertianTexture")
        material = std::make_shared<LambertianTexture>(j.at("texture").get<std::shared_ptr<Texture>>());
	else if (type == "Metal")
		material = std::make_shared<Metal>(j.at("albedo").get<Color>(), j.at("fuzz").get<double>());
	else if (type == "Dielectric")
		material = std::make_shared<Dielectric>(j.at("ior").get<double>());
    else if (type == "DiffuseLight")
        material = std::make_shared<DiffuseLight>(j.at("color").get<Color>());
    else if (type == "Isotropic")
        material = std::make_shared<Isotropic>(j.at("color").get<Color>());
    else throw std::exception(("Invalid material type: " + type).c_str());
}


// Hittable objects deserialization
void from_json(const json& j, std::shared_ptr<Hittable>& hittable)
{
    const std::string type = j.at("type").get<std::string>();

    if (type == "Sphere")
    {
        hittable = std::make_shared<Sphere>(
            j.at("center").get<Point3>(),
            j.at("radius").get<double>(),
            j.at("material").get<std::shared_ptr<Material>>());
    }
    else if (type == "MovingSphere")
    {
        hittable = std::make_shared<MovingSphere>(
            j.at("center").get<Point3>(),
            j.at("radius").get<double>(),
            j.at("direction").get<Vector3>(),
            j.at("speed").get<double>(),
            j.at("material").get<std::shared_ptr<Material>>());
    }
    else if (type == "Rectangle")
    {
        hittable = std::make_shared<Rectangle>(
            j.at("lowerCorner").get<Point3>(),
            j.at("upperCorner").get<Point3>(),
            j.at("material").get<std::shared_ptr<Material>>());
    }
    else if (type == "Box")
    {
        hittable = std::make_shared<Box>(
            j.at("lowerCorner").get<Point3>(),
            j.at("upperCorner").get<Point3>(),
            j.at("material").get<std::shared_ptr<Material>>());
    }
    else
    {
        throw std::exception(("Unsupported hittable object type: " + type).c_str());
    }

    if (j.contains("rotate_y"))
    {
        hittable = std::make_shared<Rotate_Y>(hittable, j.at("rotate_y"));
    }
    if (j.contains("translate"))
    {
        hittable = std::make_shared<Translate>(hittable, j.at("translate"));
    }
    if (j.contains("volume"))
    {
        const auto& json_volume = j.at("volume");
        const std::string volume_type = json_volume.at("type").get<std::string>();

        const auto& json_material = j.at("material");
        const std::string material_type = json_material.at("type").get<std::string>();

        if (material_type != "Isotropic")
        {
            throw std::exception("Volumetric objects must have an Isotropic material!");
        }

        if (volume_type == "ConstantMedium")
        {
            hittable = std::make_shared<ConstantMedium>(hittable,
                json_volume.at("density").get<double>(),
                json_material.at("color").get<Color>());
        }
        else
        {
            throw std::exception(("Unsupported volume type: " + volume_type).c_str());
        }
    }
}


// Scene deserialization
void from_json(const json& j, Scene& s)
{
    j.at("background").get_to<Color>(s.background);
    j.at("camera").get_to<Camera>(s.camera);
    j.at("objects").get_to<std::vector<std::shared_ptr<Hittable>>>(s.objects);
}
