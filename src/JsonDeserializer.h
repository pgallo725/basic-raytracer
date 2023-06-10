#pragma once

#include <fstream>

#include "Vector3.h"
#include "Camera.h"
#include "Hittable.h"
#include "Material.h"
#include "Scene.h"

#include "nlohmann/json.hpp"
using json = nlohmann::ordered_json;


// Forward declaration of JSON deserialization functions
void from_json(const json&, Vector3&);
void from_json(const json&, Camera&);
void from_json(const json&, std::shared_ptr<Texture>&);
void from_json(const json&, Material&);
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
        j.at("verticalFov").get<float>(),
        j.at("aperture").get<float>(),
        j.at("focusDistance").get<float>(),
        j.at("timeShutterOpen").get<float>(),
        j.at("timeShutterClose").get<float>()
    );
}


// Texture deserialization
void from_json(const json& j, std::shared_ptr<Texture>& texture)
{
    const std::string type = j.at("type").get<std::string>();

    if (type == "SolidColor")
        texture = std::make_shared<SolidTexture>(j.at("color").get<Color>());
    else if (type == "Checkerboard")
        texture = std::make_shared<CheckerTexture>(j.at("even").get<Color>(), j.at("odd").get<Color>(), j.at("scale").get<float>());
    else if (type == "Noise")
        texture = std::make_shared<NoiseTexture>(j.at("color").get<Color>(), j.at("scale").get<float>());
    else if (type == "Marble")
        texture = std::make_shared<MarbleTexture>(j.at("color").get<Color>(), j.at("scale").get<float>(), j.at("turbulence").get<float>());
    else if (type == "Image")
        texture = std::make_shared<ImageTexture>(j.at("filename").get<std::string>());
    else throw std::exception(("Invalid texture type: " + type).c_str());
}


// Material deserialization
void from_json(const json& j, Material& material)
{
	const std::string str_type = j.at("type").get<std::string>();
    const Material::Type type = Material::GetTypeFromString(str_type);

    switch (type)
    {
        case Material::Type::LambertianColor:   return Material::CreateLambertianColor(material, j.at("albedo").get<Color>());
        case Material::Type::LambertianTexture: return Material::CreateLambertianTexture(material, j.at("texture").get<std::shared_ptr<Texture>>());
        case Material::Type::Metal:             return Material::CreateMetal(material, j.at("albedo").get<Color>(), j.at("fuzz").get<float>());
        case Material::Type::Dielectric:        return Material::CreateDielectric(material, j.at("ior").get<float>());
        case Material::Type::DiffuseLight:      return Material::CreateDiffuseLight(material, j.at("color").get<Color>());
        case Material::Type::Isotropic:         return Material::CreateIsotropic(material, j.at("color").get<Color>());
        default:                                throw std::exception(("Invalid material type: " + str_type).c_str());
    }
}


// Scene deserialization
void from_json(const json& j, Scene& s)
{
    j.at("background").get_to<Color>(s.background);
    j.at("camera").get_to<Camera>(s.camera);

    const auto& objects = j.at("objects");
    if (!objects.is_array())
        return;

    s.materials.Allocate(objects.size());
    s.objects.Allocate(objects.size());

    // Objects deserialization
    for (size_t i = 0; i < objects.size(); i++)
    {
        const json& object = objects[i];

        if (object.contains("material"))
            object.at("material").get_to<Material>(s.materials[i]);
        else throw std::exception("Objects without materials are not supported!");

        const std::string str_type = object.at("type").get<std::string>();
        const Hittable::Type type = Hittable::GetTypeFromString(str_type);

        Hittable hittable;
        switch (type)
        {
            case Hittable::Type::Sphere:
                Hittable::CreateSphere(hittable, object.at("center").get<Point3>(), object.at("radius").get<float>());
                break;
            case Hittable::Type::MovingSphere:
                Hittable::CreateMovingSphere(hittable, object.at("center").get<Point3>(), object.at("radius").get<float>(),
                    object.at("direction").get<Vector3>(), object.at("speed").get<float>());
                break;
            case Hittable::Type::Rectangle:
                Hittable::CreateRectangle(hittable, object.at("lowerCorner").get<Point3>(), object.at("upperCorner").get<Point3>());
                break;
            case Hittable::Type::Box:
                Hittable::CreateBox(hittable, object.at("lowerCorner").get<Point3>(), object.at("upperCorner").get<Point3>());
                break;
            default:
                throw std::exception(("Unsupported hittable object type: " + str_type).c_str());
        }
        if (object.contains("translate"))
        {
            hittable.translation = object.at("translate");
        }
        if (object.contains("rotate_y"))
        {
            hittable.rotation_y = Hittable::Rotation(object.at("rotate_y"));
        }
        if (object.contains("volume"))
        {
            const auto& json_volume = object.at("volume");
            const std::string volume_type = json_volume.at("type").get<std::string>();

            const auto& json_material = object.at("material");
            const std::string material_type = json_material.at("type").get<std::string>();

            if (material_type != "Isotropic")
            {
                throw std::exception("Volumetric objects must have an Isotropic material!");
            }

            if (volume_type == "ConstantMedium")
            {
                std::shared_ptr<const Hittable> boundary = std::make_shared<Hittable>(hittable);
                hittable = {};  // reset hittable
                Hittable::CreateVolume(hittable, boundary, json_volume.at("density").get<float>());
            }
            else
            {
                throw std::exception(("Unsupported volume type: " + volume_type).c_str());
            }
        }
        hittable.material = MaterialID(i);
        s.objects[i] = hittable;
    }
}
