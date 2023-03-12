#pragma once

#include <fstream>

#include "Vector3.h"
#include "Material.h"
#include "Sphere.h"
#include "MovingSphere.h"
#include "Camera.h"
#include "Scene.h"

#include "nlohmann/json.hpp"
using json = nlohmann::ordered_json;


// Forward declaration of JSON deserialization functions
void from_json(const json&, Vector3&);
void from_json(const json&, std::shared_ptr<Material>&);
void from_json(const json&, Camera&);
void from_json(const json&, Sphere&);
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


// Material deserialization
void from_json(const json& j, std::shared_ptr<Material>& m)
{
	std::string type = j.at("type").get<std::string>();

	if (type.compare("Lambertian") == 0)
        m = std::make_shared<Lambertian>(j.at("albedo").get<Color>());
	else if (type.compare("Metal") == 0)
		m = std::make_shared<Metal>(j.at("albedo").get<Color>(), j.at("fuzz").get<double>());
	else if (type.compare("Dielectric") == 0)
		m = std::make_shared<Dielectric>(j.at("ior").get<double>());
    else throw std::exception(("Invalid material type: " + type).c_str());
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


// Sphere deserialization
void from_json(const json& j, Sphere& s)
{
    j.at("center").get_to<Point3>(s.center);
    j.at("radius").get_to<double>(s.radius);
    j.at("material").get_to<std::shared_ptr<Material>>(s.material);
}


// MovingSphere deserialization
void from_json(const json& j, MovingSphere& ms)
{
    j.at("center").get_to<Point3>(ms.center);
    j.at("radius").get_to<double>(ms.radius);
    j.at("direction").get_to<Vector3>(ms.direction);
    j.at("speed").get_to<double>(ms.speed);
    j.at("material").get_to<std::shared_ptr<Material>>(ms.material);
}


// Scene deserialization
void from_json(const json& j, Scene& s)
{
    j.at("camera").get_to<Camera>(s.camera);
    j.at("spheres").get_to<std::vector<Sphere>>(s.spheres);
    j.at("movingSpheres").get_to<std::vector<MovingSphere>>(s.moving_spheres);
}
