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
void from_json(const json&, std::shared_ptr<Texture>&);
void from_json(const json&, std::shared_ptr<Material>&);
void from_json(const json&, Camera&);
void from_json(const json&, std::shared_ptr<Sphere>&);
void from_json(const json&, std::shared_ptr<MovingSphere>&);
void from_json(const json&, std::shared_ptr<Rectangle>&);
void from_json(const json&, std::shared_ptr<Box>&);
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


// Texture deserialization
void from_json(const json& j, std::shared_ptr<Texture>& t)
{
    std::string type = j.at("type").get<std::string>();

    if (type.compare("SolidColor") == 0)
        t = std::make_shared<SolidTexture>(j.at("color").get<Color>());
    else if (type.compare("Checkerboard") == 0)
        t = std::make_shared<CheckerTexture>(j.at("even").get<Color>(), j.at("odd").get<Color>(), j.at("scale").get<double>());
    else if (type.compare("Noise") == 0)
        t = std::make_shared<NoiseTexture>(j.at("color").get<Color>(), j.at("scale").get<double>());
    else if (type.compare("Marble") == 0)
        t = std::make_shared<MarbleTexture>(j.at("color").get<Color>(), j.at("scale").get<double>(), j.at("turbulence").get<double>());
    else if (type.compare("Image") == 0)
        t = std::make_shared<ImageTexture>(j.at("filename").get<std::string>());
    else throw std::exception(("Invalid texture type: " + type).c_str());
}


// Material deserialization
void from_json(const json& j, std::shared_ptr<Material>& m)
{
	std::string type = j.at("type").get<std::string>();

	if (type.compare("LambertianColor") == 0)
        m = std::make_shared<LambertianColor>(j.at("albedo").get<Color>());
    else if (type.compare("LambertianTexture") == 0)
        m = std::make_shared<LambertianTexture>(j.at("texture").get<std::shared_ptr<Texture>>());
	else if (type.compare("Metal") == 0)
		m = std::make_shared<Metal>(j.at("albedo").get<Color>(), j.at("fuzz").get<double>());
	else if (type.compare("Dielectric") == 0)
		m = std::make_shared<Dielectric>(j.at("ior").get<double>());
    else if (type.compare("DiffuseLight") == 0)
        m = std::make_shared<DiffuseLight>(j.at("color").get<Color>());
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
void from_json(const json& j, std::shared_ptr<Sphere>& s)
{
    s = std::make_shared<Sphere>(j.at("center").get<Point3>(),
                                 j.at("radius").get<double>(),
                                 j.at("material").get<std::shared_ptr<Material>>());
}


// MovingSphere deserialization
void from_json(const json& j, std::shared_ptr<MovingSphere>& ms)
{
    ms = std::make_shared<MovingSphere>(j.at("center").get<Point3>(),
                                        j.at("radius").get<double>(),
                                        j.at("direction").get<Vector3>(),
                                        j.at("speed").get<double>(),
                                        j.at("material").get<std::shared_ptr<Material>>());
}


// Rectangle deserialization
void from_json(const json& j, std::shared_ptr<Rectangle>& ms)
{
    ms = std::make_shared<Rectangle>(j.at("firstCorner").get<Point3>(),
                                     j.at("secondCorner").get<Point3>(),
                                     j.at("material").get<std::shared_ptr<Material>>());
}


// Box deserialization
void from_json(const json& j, std::shared_ptr<Box>& ms)
{
    ms = std::make_shared<Box>(j.at("lowerCorner").get<Point3>(),
                               j.at("upperCorner").get<Point3>(),
                               j.at("material").get<std::shared_ptr<Material>>());
}


// Scene deserialization
void from_json(const json& j, Scene& s)
{
    j.at("background").get_to<Color>(s.background);
    j.at("camera").get_to<Camera>(s.camera);
    
    auto spheres = j.at("spheres").get<std::vector<std::shared_ptr<Sphere>>>();
    auto movingSpheres = j.at("movingSpheres").get<std::vector<std::shared_ptr<MovingSphere>>>();
    auto rectangles = j.at("rectangles").get<std::vector<std::shared_ptr<Rectangle>>>();
    auto boxes = j.at("boxes").get<std::vector<std::shared_ptr<Box>>>();

    for (const auto& sphere : spheres)
        s.objects.push_back(sphere);
    for (const auto& movingSphere : movingSpheres)
        s.objects.push_back(movingSphere);
    for (const auto& rectangle : rectangles)
        s.objects.push_back(rectangle);
    for (const auto& box : boxes)
        s.objects.push_back(box);

#ifdef noact
    // Custom Cornell Box code
    std::shared_ptr<Material> white = std::make_shared<LambertianColor>(Color(0.73, 0.73, 0.73));
    std::shared_ptr<Hittable> box1 = std::make_shared<Box>(Point3(0, 0, 0), Point3(165, 330, 165), white);
    box1 = std::make_shared<Rotate_Y>(box1, 15);
    box1 = std::make_shared<Translate>(box1, Vector3(265, 0, 295));
    s.objects.push_back(std::make_shared<ConstantMedium>(box1, 0.01, Color(0, 0, 0)));
    std::shared_ptr<Hittable> box2 = std::make_shared<Box>(Point3(0, 0, 0), Point3(165, 165, 165), white);
    box2 = std::make_shared<Rotate_Y>(box2, -18);
    box2 = std::make_shared<Translate>(box2, Vector3(130, 0, 65));
    s.objects.push_back(std::make_shared<ConstantMedium>(box2, 0.01, Color(1, 1, 1)));
#endif
}
