#include <X11/Xlib.h>
#include <math.h>
#include <vector>
#include <string>

struct Vec3
{
	double x, y, z;

	Vec3(){};

	Vec3(double x, double y, double z) : x(x), y(y), z(z)
	{
	}

	Vec3 operator+(const Vec3 &v) const
	{
		return Vec3(x + v.x, y + v.y, z + v.z);
	}

	Vec3 operator+(const int num) const
	{
		return Vec3(x + num, y + num, z + num);
	}

	Vec3 operator-(const Vec3 &v) const
	{
		return Vec3(x - v.x, y - v.y, z - v.z);
	}

	Vec3 operator-(const double val) const
	{
		return Vec3(x - val, y - val, z - val);
	}

	Vec3 operator*(double mul) const
	{
		return Vec3(x * mul, y * mul, z * mul);
	}

	Vec3 operator*(const Vec3 v) const
	{
		return Vec3(v.x * x, v.y * y, v.z * z);
	}

	Vec3 operator/(double div) const
	{
		return Vec3(x / div, y / div, z / div);
	}

	Vec3 operator/(const Vec3 &div) const
	{
		return Vec3(x / div.x, y / div.y, z / div.z);
	}

	bool operator==(const Vec3 &v1)
	{
		if (v1.x == x && v1.y == y && v1.z == z)
			return true;

		return false;
	}

	bool operator!=(const Vec3 &v1)
	{
		if (v1.x != x && v1.y != y && v1.z != z)
			return true;

		return false;
	}

	bool operator>(const Vec3 &v1)
	{
		if (v1.x > x && v1.y > y && v1.z > z)
			return true;

		return false;
	}

	bool operator<(const Vec3 &v1)
	{
		if (v1.x < x && v1.y < y && v1.z < z)
			return true;

		return false;
	}

	Vec3 Normalize() const
	{
		double magnitude = sqrt(x * x + y * y + z * z);
		return Vec3(x / magnitude, y / magnitude, z / magnitude);
	}
};

inline double dot(const Vec3 &vec1, const Vec3 &vec2)
{
	return (vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z);
}

struct Ray
{
	Vec3 origin, destination;
	Ray(const Vec3 &org, const Vec3 &dest) : origin(org), destination(dest)
	{
	}
};

struct Sphere
{
	Vec3 center;
	double radius;
	Vec3 clr;
	std::string name;

	Sphere(const Vec3 &ctr, double rad, Vec3 &clr) : center(ctr), radius(rad), clr(clr)
	{
	}

	Sphere(const Vec3 &ctr, double rad, Vec3 &clr, std::string name) : center(ctr), radius(rad), clr(clr), name(name)
	{
	}

	Vec3 GetNormal(const Vec3 &pi) const
	{
		return (pi - center) / radius;
	}

	bool Intersects(const Ray &ray, double &t) const
	{
		const Vec3 origin = ray.origin;
		const Vec3 dest = ray.destination;
		const Vec3 orgCtr = origin - this->center;
		const double b = 2 * dot(orgCtr, dest);
		const double center = dot(orgCtr, orgCtr) - radius * radius;
		double discrim = b * b - 4 * center;

		if (discrim < 1e-4)
			return false;

		discrim = sqrt(discrim);
		const double t0 = -b - discrim;
		const double t1 = -b + discrim;

		t = (t0 < t1) ? t0 : t1;

		return true;
	}

	bool operator!=(const Sphere &s) const
	{
		if (s.center.x == center.x && s.center.y == center.y && s.center.z == center.z)
			return false;

		return true;
	}

	bool operator==(const Sphere &s) const
	{
		if (s.center.x == center.x && s.center.y == center.y && s.center.z == center.z)
			return true;

		return false;
	}
};

struct Colors
{
	Vec3 white = {255, 255, 255};
	Vec3 yellow = {255, 255, 0};
	Vec3 black = {0, 0, 0};
	Vec3 red = {255, 0, 110};
	Vec3 green = {110, 255, 0};
	Vec3 blue = {0, 110, 255};
};

void ColorBoundary(Vec3 &clr)
{
	clr.x = (clr.x > 255) ? 255 : (clr.x < 0) ? 0 : clr.x;
	clr.y = (clr.y > 255) ? 255 : (clr.y < 0) ? 0 : clr.y;
	clr.z = (clr.z > 255) ? 255 : (clr.z < 0) ? 0 : clr.z;
}

void Reflection(const std::vector<Sphere *> &objects, const Vec3 &pointOfIntersection, const Vec3 &normal, Vec3 &pixclr)
{
	Colors color;
	double reflectionIntensity = 2.5;

	for (auto &sphereB : objects)
	{
		const Ray ray2(pointOfIntersection, normal * 40);
		double t2;
		if (sphereB->Intersects(ray2, t2))
		{
			const Vec3 pointOfIntersection2 = ray2.origin + ray2.destination * t2;
			const Vec3 len2 = pointOfIntersection - pointOfIntersection2;
			const Vec3 normal2 = sphereB->GetNormal(pointOfIntersection2);
			const double dotP2 = dot(len2.Normalize(), normal2.Normalize());

			Vec3 pixclrBounce = sphereB->clr * dotP2 * reflectionIntensity;
			if (sphereB->name == "world")
				pixclrBounce = pixclrBounce - color.white / 1.5;

			ColorBoundary(pixclrBounce);
			pixclr = pixclrBounce + pixclr;
		}
	}
}

void Shadow(const std::vector<Sphere *> &objects, const Vec3 &pointOfIntersection, const Vec3 &normal, Vec3 &pixclr,
			const Sphere &light)
{
	Colors color;

	double shadowIntensity = 2.3;

	for (auto &sphereB : objects)
	{
		const Ray ray2(pointOfIntersection, light.GetNormal(sphereB->center) * -3);
		double t2;
		if (sphereB->Intersects(ray2, t2))
		{
			const Vec3 pointOfIntersection2 = ray2.origin + ray2.destination * t2;
			const Vec3 len2 = pointOfIntersection - pointOfIntersection2;
			const Vec3 normal2 = sphereB->GetNormal(pointOfIntersection2);
			const double dotP2 = dot(len2.Normalize(), normal2.Normalize());

			Vec3 shadow = color.black + 1 * shadowIntensity;
			Vec3 pixclrBounce = pixclr * dotP2 * shadow;
			ColorBoundary(pixclrBounce);

			pixclr = pixclr - pixclrBounce;
		}
	}
}

Vec3 Trace(const Ray &ray, const Sphere &light, const std::vector<Sphere *> &objects)
{
	Colors color;
	Vec3 pixclr = color.black;

	for (auto &sphere : objects)
	{
		double t;
		double lightIntensity = 0.62;

		if (sphere->Intersects(ray, t))
		{
			const Vec3 pointOfIntersection = ray.origin + ray.destination * t;
			const Vec3 len = light.center - pointOfIntersection;
			const Vec3 normal = sphere->GetNormal(pointOfIntersection);
			const double dotProduct = dot(len.Normalize(), normal.Normalize());

			pixclr = (sphere->clr + light.clr / 2 * dotProduct) * lightIntensity;

			if (sphere->name != "world")
				Reflection(objects, pointOfIntersection, normal, pixclr);
			Shadow(objects, pointOfIntersection, normal, pixclr, light);

			ColorBoundary(pixclr);
		}
	}

	return pixclr;
}

struct ScreenData
{
	Colors &color;
	int x, y;
	Sphere &light;
	std::vector<Sphere *> &objList;
};

Vec3 RenderPixel(const ScreenData &sd)
{
	Vec3 pixclr = sd.color.black;
	//~ Vec3 pixclrBounce = sd.color.black;

	const Ray ray(Vec3(sd.x, sd.y, -3), Vec3(0, 0, 1));
	pixclr = Trace(ray, sd.light, sd.objList);

	return pixclr;
}

void HandleEvents(XEvent &event, Sphere &light, Sphere &world)
{
	if (event.type == MotionNotify)
	{
		light.center.x = event.xmotion.x;
		light.center.y = event.xmotion.y;
	}

	if (event.type == ButtonPress && event.xbutton.button == 4 && world.center.z > 580)
	{
		world.center.z -= 5;
		world.center.y -= 5;

		light.center.z -= 30;
	}

	if (event.type == ButtonPress && event.xbutton.button == 5 && world.center.z < 600)
	{
		world.center.z += 5;
		world.center.y += 5;

		light.center.z += 30;
	}
}

inline unsigned long _RGB(int r, int g, int b)
{
	return b + (g << 8) + (r << 16);
}

int main()
{
	const int W = 300;
	const int H = 300;

	//
	Display *display = XOpenDisplay(NULL);
	Window rWin = XDefaultRootWindow(display);

	Window win = XCreateSimpleWindow(display, rWin, 0, 0, W, H, 0, 0, 0);

	XStoreName(display, win, "Ray Tracer");

	unsigned long eventmask = PointerMotionMask | ButtonPressMask;
	XSelectInput(display, win, eventmask);

	XMapWindow(display, win);

	// TODO: render style
	GC render = XCreateGC(display, win, 0, NULL);
	XSync(display, False);

	// WORLD

	Colors color;

	Sphere sphere(Vec3(W * 0.5, H * 0.5, 40), 30, color.white);
	Sphere sphere1(Vec3(W * 0.3, H * 0.5, 20), 15, color.red);
	Sphere sphere2(Vec3(W * 0.25, H * 0.35, 30), 20, color.green);
	Sphere sphere3(Vec3(W * 0.85, H * 0.65, 40), 35, color.blue);

	Vec3 worldColor = Vec3(94, 0, 182);
	Sphere world(Vec3(W * 0.5, H * 3.8, 650), 1100, worldColor, "world");

	Sphere light(Vec3(W * 0.1, H * 0.5, 0), 40, color.white);

	std::vector<Sphere *> objList = {&world, &sphere, &sphere1, &sphere2, &sphere3};

	double theta = 0.0;

	std::vector<Vec3> background;
	for (int y = 0; y < H * 2; ++y)
		for (int x = 0; x < W * 2; ++x)
			background.emplace_back(Vec3(122, 11, 133) - (x + y) / 5);

	// WORLD LOOP

	XEvent event;
	while (1)
	{
		if (!XCheckMaskEvent(display, eventmask, &event))
			event.type = 0;

		HandleEvents(event, light, world);

		for (int y = 0; y < H; ++y)
			for (int x = 0; x < W; ++x)
			{
				ScreenData scrnData = {color, x, y, light, objList};
				Vec3 pixclr = RenderPixel(scrnData);
				ColorBoundary(pixclr);

				XSetForeground(display, render, _RGB((int)pixclr.x, (int)pixclr.y, (int)pixclr.z));
				XDrawPoint(display, win, render, x, y);
			}

		int startIdx = 1;
		objList[startIdx]->center.x = (W >> 1) + (W / 6) * sin(theta);
		objList[startIdx]->center.y = H / 2.8 + (H / 6) * cos(theta / 2);
		objList[startIdx + 1]->center.x = objList[startIdx + 1]->center.x + 2 * sin(theta);
		objList[startIdx + 1]->center.y = objList[startIdx + 1]->center.y + 2 * cos(theta);
		objList[startIdx + 2]->center.y = objList[startIdx + 2]->center.y + 2 * sin(theta);
		objList[startIdx + 3]->center.y = objList[startIdx + 3]->center.y + 2 * -sin(theta);
		// objList[startIdx + 3]->center.z = 50 + 10 * sin(theta);

		theta += 0.1;

		XFlush(display);
		XSync(display, False);
	}

	XCloseDisplay(display);
	return 0;
}
