#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace sf;

struct Point {
    float x, y;
};

float catmullRom(float p0, float p1, float p2, float p3, float t) {
    return 0.5f * ((2 * p1) +
                  (-p0 + p2) * t +
                  (2 * p0 - 5 * p1 + 4 * p2 - p3) * t * t +
                  (-p0 + 3 * p1 - 3 * p2 + p3) * t * t * t);
}

class Terrain {
public:
    std::vector<Point> controlPoints;
    VertexArray shape;
    float screenHeight;
    float totalWidth;
    float spacing;
    int smoothness;

    Terrain(float screenH, float width, float pointSpacing, int smooth)
        : screenHeight(screenH), totalWidth(width), spacing(pointSpacing), smoothness(smooth) {
        shape.setPrimitiveType(TriangleStrip);
        generatePoints();
    }

    void generatePoints() {
        controlPoints.clear();
        srand(time(nullptr));

        float buffer = spacing * 4; 

        for (float x = -buffer; x <= totalWidth + buffer; x += spacing) {
            float y = 500 + (rand() % 200 - 100);
            controlPoints.push_back({x, y});
        }
    }

    void update(float scrollX, float windowWidth) {
        shape.clear();

        for (size_t i = 1; i < controlPoints.size() - 2; ++i) {
            Point p0 = controlPoints[i - 1];
            Point p1 = controlPoints[i];
            Point p2 = controlPoints[i + 1];
            Point p3 = controlPoints[i + 2];

            for (int j = 0; j < smoothness; ++j) {
                float t = j / static_cast<float>(smoothness);
                float x = catmullRom(p0.x, p1.x, p2.x, p3.x, t) - scrollX;
                float y = catmullRom(p0.y, p1.y, p2.y, p3.y, t);

            
                if (x < -50 || x > windowWidth + 50) continue;

                Vertex top(Vector2f(x, y), Color(128, 128, 128));
                Vertex bottom(Vector2f(x, screenHeight), Color(128, 128, 128));
                shape.append(top);
                shape.append(bottom);
            }
        }
    }

    void draw(RenderWindow& window) {
        window.draw(shape);
    }
};

int main() {
    RenderWindow window(VideoMode(1200, 700), "Random Spline Terrain");

   
    Texture bgTexture;
    if (!bgTexture.loadFromFile("assets/fuji.png")) {
        return -1;
    }
    Sprite background(bgTexture);
    Vector2u winSize = window.getSize();
    background.setScale(
        float(winSize.x) / bgTexture.getSize().x,
        float(winSize.y) / bgTexture.getSize().y
    );

    
    Terrain terrain(700, 5000.f, 100.f, 20);

    float scrollX = 0.f;
    float scrollSpeed = 0.1;
    const float maxScroll = 5000 - winSize.x;

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }

        if (Keyboard::isKeyPressed(Keyboard::Right)) scrollX += scrollSpeed;
        if (Keyboard::isKeyPressed(Keyboard::Left)) scrollX -= scrollSpeed;

        if (scrollX < 0) scrollX = 0;
        if (scrollX > maxScroll) scrollX = maxScroll;

        terrain.update(scrollX, winSize.x);

        window.clear();
        window.draw(background);
        terrain.draw(window);
        window.display();
    }

    return 0;
}
