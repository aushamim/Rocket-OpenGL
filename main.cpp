#include <bits/stdc++.h>
#include <windows.h>
#include <GL/glut.h>
#include <GL/gl.h>
using namespace std;

// Screen Resolution
int screenResX = GetSystemMetrics(SM_CXSCREEN);

// Global Animation Parameters
int animState = 1;
float starDepth1Speed = screenResX < 1920 ? 0.03 : 0.9;
float starDepth2Speed = screenResX < 1920 ? 0.02 : 0.8;
float starDepth3Speed = screenResX < 1920 ? 0.01 : 0.7;
float rockSpeed = screenResX < 1920 ? 0.002 : 0.5;
float laserSpeed = screenResX < 1920 ? 0.002 : 0.5;

// Global Parameters
long long startTime = time(NULL);
int prevSec = 0;
int prevSec2 = 0;
int score = 0;
float boostAvailable = 100.00;
int starCount = 50;
float starPos[6][50]; // X1, Y1, X2, Y2, X3, Y3
int rockCount = 10;
float rockPos[3][10];         // X, Y, Scale
float laserPos[3][200] = {0}; // X, Y, Closest Rock
int laserNextIndex = 0;
float rocketX = 280;
int rocketBoosting = 0;

void resetSpeed()
{
    rocketBoosting = 0;
    starDepth1Speed = screenResX < 1920 ? 0.03 : 0.9;
    starDepth2Speed = screenResX < 1920 ? 0.02 : 0.8;
    starDepth3Speed = screenResX < 1920 ? 0.01 : 0.7;
    rockSpeed = screenResX < 1920 ? 0.02 : 0.5;
}

void init(void)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glClearColor(0.0, 0.0, 0.0, 0.0); // GLfloat red,green,blue,alpha initial value 0 alpha values used by glclear to clear the color buffers
    glMatrixMode(GL_PROJECTION);      // To specify which matrix is the current matrix & projection applies subsequent matrix to projecton matrix stack
    glLoadIdentity();
    glOrtho(0.0, 500.0, 0.0, 700.0, -1.0, 1.0); // Left, Right, Bottom, Top, Near, Far
    // gluOrtho2D(-300.0,300.0,-300.0,300.0); // Left, Right, Bottom, Top, Near, Far
}

float rgb(float rgb)
{
    float x = rgb / 255;
    return floorf(x * 100) / 100;
}

void star(GLfloat positionX, GLfloat positionY, GLfloat alpha)
{
    glColor4f(rgb(255), rgb(255), rgb(255), alpha);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(positionX, positionY);
    for (int i = 0; i <= 10; i++)
    {
        float angle = 2.0f * 3.1416f * i / 10;
        float x = 1 * cosf(angle);
        float y = 1 * sinf(angle);
        glVertex2f(x + positionX, y + positionY);
    }
    glEnd();
}

float curve(float A, float B, float C, float t)
{
    float s = 1 - t;
    float AB = A * s + B * t;
    float BC = B * s + C * t;
    float ABC = AB * s + BC * t;
    return ABC;
}

void rocket()
{
    float t = 0;

    // Flames
    glColor4f(rgb(rocketBoosting == 0 ? 254 : 166), rgb(rocketBoosting == 0 ? 108 : 166), rgb(rocketBoosting == 0 ? 2 : 242), 1);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(rocketX - 30, 110);
    for (int i = 0; i <= 10; i++)
    {
        float angle = 2.0f * 3.1416f * i / 10;
        float x = 9 * cosf(angle);
        float y = 35 * sinf(angle);
        glVertex2f(x + rocketX - 30, y + 110);
    }
    glEnd();
    glColor4f(rgb(rocketBoosting == 0 ? 255 : 95), rgb(rocketBoosting == 0 ? 163 : 94), rgb(rocketBoosting == 0 ? 0 : 233), 1);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(rocketX - 30, 110);
    for (int i = 0; i <= 10; i++)
    {
        float angle = 2.0f * 3.1416f * i / 10;
        float x = 6 * cosf(angle);
        float y = 25 * sinf(angle);
        glVertex2f(x + rocketX - 30, y + 110);
    }
    glEnd();

    // Fins
    glColor4f(rgb(249), rgb(59), rgb(43), 1);
    glBegin(GL_POLYGON);
    glVertex2f(rocketX - 20, 110);
    glVertex2f(rocketX, 100);
    t = 0;
    while (t < 1)
    {
        float x = curve(rocketX, rocketX, rocketX - 20, t);
        float y = curve(100, 120, 130, t);
        glVertex2f(x, y);
        t += 0.1;
    }
    glVertex2f(rocketX - 20, 130);
    glVertex2f(rocketX - 40, 130);
    t = 0;
    while (t < 1)
    {
        float x = curve(rocketX - 40, rocketX - 60, rocketX - 60, t);
        float y = curve(130, 120, 100, t);
        glVertex2f(x, y);
        t += 0.1;
    }
    glVertex2f(rocketX - 60, 100);
    glEnd();

    // Body
    glColor4f(rgb(230), rgb(242), rgb(251), 1);
    glBegin(GL_POLYGON);
    glVertex2f(rocketX - 20, 100);
    t = 0;
    while (t < 1)
    {
        float x = curve(rocketX - 20, rocketX, rocketX - 30, t);
        float y = curve(100, 140, 180, t);
        glVertex2f(x, y);
        t += 0.1;
    }
    t = 0;
    while (t < 1)
    {
        float x = curve(rocketX - 30, rocketX - 60, rocketX - 40, t);
        float y = curve(180, 140, 100, t);
        glVertex2f(x, y);
        t += 0.1;
    }
    glVertex2f(rocketX - 40, 100);
    glEnd();

    // Window
    glColor4f(rgb(22), rgb(110), rgb(124), 1);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(rocketX - 30, 150);
    for (int i = 0; i <= 10; i++)
    {
        float angle = 2.0f * 3.1416f * i / 10;
        float x = 11 * cosf(angle);
        float y = 11 * sinf(angle);
        glVertex2f(x + rocketX - 30, y + 150);
    }
    glEnd();
    glColor4f(rgb(37), rgb(179), rgb(201), 1);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(rocketX - 30, 150);
    for (int i = 0; i <= 10; i++)
    {
        float angle = 2.0f * 3.1416f * i / 10;
        float x = 8 * cosf(angle);
        float y = 8 * sinf(angle);
        glVertex2f(x + rocketX - 30, y + 150);
    }
    glEnd();
}

void rock(GLfloat x, GLfloat y, GLfloat s)
{
    glColor4f(rgb(132), rgb(96), rgb(71), 1);
    glBegin(GL_POLYGON);
    glVertex2f(20 + x, 0 + y);
    glVertex2f(30 + x, 20 + y);
    glVertex2f(20 + x - s, 40 + y - s);
    glVertex2f(0 + x, 30 + y);
    glVertex2f(0 + x, 10 + y);
    glEnd();

    glColor4f(rgb(154), rgb(112), rgb(83), 1);
    glBegin(GL_POLYGON);
    glVertex2f(20 + x, 5 + y);
    glVertex2f(25 + x, 20 + y);
    glVertex2f(15 + x - s, 35 + y - s);
    glVertex2f(5 + x, 25 + y);
    glVertex2f(5 + x, 15 + y);
    glEnd();

    glColor4f(rgb(132), rgb(96), rgb(71), 1);
    glBegin(GL_TRIANGLES);
    glVertex2f(23 + x, 5 + y);
    glVertex2f(25 + x - s, 15 + y + s);
    glVertex2f(15 + x, 20 + y);
    glEnd();
}

void laser(GLfloat x, GLfloat y)
{
    glLineWidth(5);
    glColor4f(rgb(245), rgb(77), rgb(224), 1);
    glBegin(GL_LINES);
    glVertex2f(x, y);
    glColor4f(rgb(58), rgb(97), rgb(234), 1);
    glVertex2f(x, y + 20);
    glEnd();
}

void consoleOutput()
{
    system("cls");
    cout << "-----------------\t\tBoost: " << setprecision(0) << fixed << boostAvailable << "%   ";
    for (int i = 0; i <= boostAvailable; i += 10)
    {
        cout << (char)219u;
    }
    cout << endl;
    cout << "|     | ^ |     |\t\tScore: " << score << endl;
    cout << "|     | 8 |     |" << endl;
    cout << "------|---|------" << endl;
    cout << "| < 4 | 5 | 6 > |" << endl;
    cout << "------|---|------" << endl;
    cout << "|     | 2 |     |" << endl;
    cout << "|     | v |     |" << endl;
    cout << "-----------------" << endl;
    cout << endl
         << endl;
    cout << "Key Press" << endl;
    cout << "2 -> Boost OFF" << endl;
    cout << "4 -> Go Left" << endl;
    cout << "5 -> Reset" << endl;
    cout << "6 -> Go Right" << endl;
    cout << "8 -> Boost ON" << endl;
    cout << "0, SPACE -> Fire Laser" << endl;
    cout << endl
         << endl;

    if (animState == 0)
    {
        if (screenResX < 1920)
        {
            cout << "   _____          __  __ ______    ______      ________ _____  " << endl;
            cout << "  / ____|   /\\   |  \\/  |  ____|  / __ \\ \\    / /  ____|  __ \\ " << endl;
            cout << " | |  __   /  \\  | \\  / | |__    | |  | \\ \\  / /| |__  | |__) |" << endl;
            cout << " | | |_ | / /\\ \\ | |\\/| |  __|   | |  | |\\ \\/ / |  __| |  _  / " << endl;
            cout << " | |__| |/ ____ \\| |  | | |____  | |__| | \\  /  | |____| | \\ \\ " << endl;
            cout << "  \\_____/_/    \\_\\_|  |_|______|  \\____/   \\/   |______|_|  \\_\\" << endl;
            cout << "                                                               " << endl;
        }
        else
        {
            cout << "     _______      ___      .___  ___.  _______            ______   ____    ____  _______ .______      " << endl;
            cout << "    /  _____|    /   \\     |   \\/   | |   ____|          /  __  \\  \\   \\  /   / |   ____||   _  \\     " << endl;
            cout << "   |  |  __     /  ^  \\    |  \\  /  | |  |__            |  |  |  |  \\   \\/   /  |  |__   |  |_)  |    " << endl;
            cout << "   |  | |_ |   /  /_\\  \\   |  |\\/|  | |   __|           |  |  |  |   \\      /   |   __|  |      /     " << endl;
            cout << "   |  |__| |  /  _____  \\  |  |  |  | |  |____          |  `--'  |    \\    /    |  |____ |  |\\  \\----." << endl;
            cout << "    \\______| /__/     \\__\\ |__|  |__| |_______|          \\______/      \\__/     |_______|| _| `._____|" << endl;
            cout << "                                                                                                      " << endl;
        }
    }
}

void keyPressed(unsigned char key, int x, int y)
{
    if (key == '8' && animState == 1 && rocketBoosting == 0)
    {
        rocketBoosting = 1;
        starDepth1Speed += screenResX < 1920 ? 0.04 : 1.6;
        starDepth2Speed += screenResX < 1920 ? 0.04 : 1.6;
        starDepth3Speed += screenResX < 1920 ? 0.04 : 1.6;
        rockSpeed += screenResX < 1920 ? 0.03 : 1;
    }
    if (key == '2' && animState == 1 && rocketBoosting == 1)
    {
        resetSpeed();
    }
    if (key == '4' && rocketX > 70 && animState == 1)
    {
        rocketX -= 5;
    }
    if (key == '6' && rocketX < 490 && animState == 1)
    {
        rocketX += 5;
    }
    if (key == '5')
    {
        animState = 1;
    }
    if ((key == '0' || key == ' ') && animState == 1)
    {
        laserPos[0][laserNextIndex] = rocketX - 30;
        laserPos[1][laserNextIndex] = 180;
        float closestRockToLaser = abs(rockPos[0][0] - laserPos[0][laserNextIndex]);
        for (int i = 1; i < rockCount; i++)
        {
            if (abs(rockPos[0][i] - laserPos[0][laserNextIndex]) < closestRockToLaser && rockPos[1][i] < 720 && rockPos[1][i] > 180)
            {
                closestRockToLaser = abs(rockPos[0][i] - laserPos[0][laserNextIndex]);
                laserPos[2][laserNextIndex] = i;
            }
        }
        laserNextIndex <= 200 ? laserNextIndex++ : laserNextIndex = 0;
    }
}

void Draw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Element Section

    // Stars
    for (int i = 0; i < starCount; i++)
    {
        star(starPos[0][i], starPos[1][i], 0.8);
        star(starPos[2][i], starPos[3][i], 0.5);
        star(starPos[4][i], starPos[5][i], 0.2);
    }

    // Laser
    for (int i = 0; i < 200; i++)
    {
        if (laserPos[0][i] > 0)
        {
            laser(laserPos[0][i], laserPos[1][i]);
        }
    }

    // Rocks
    for (int i = 0; i < rockCount; i++)
    {
        rock(rockPos[0][i], rockPos[1][i], rockPos[2][i]);
    }

    // Rocket
    rocket();

    // ------------------------------------------------------------------------- //

    // Animation Section
    if (animState == 1)
    {
        // Stars
        for (int i = 0; i < starCount; i++)
        {
            starPos[1][i] >= 0 ? starPos[1][i] -= starDepth1Speed : starPos[1][i] = 1400;
            starPos[3][i] >= 0 ? starPos[3][i] -= starDepth2Speed : starPos[3][i] = 1400;
            starPos[5][i] >= 0 ? starPos[5][i] -= starDepth3Speed : starPos[5][i] = 1400;
        }
        // Rocks
        for (int i = 0; i < rockCount; i++)
        {
            rockPos[1][i] > -50 ? rockPos[1][i] -= rockSpeed : rockPos[1][i] = 1000 + rand() % 400;
            rockPos[1][i] < -50 ? rockPos[0][i] = rand() % 400 : rockPos[0][i];
        }
        // Laser
        for (int i = 0; i < 200; i++)
        {
            laserPos[1][i] < 720 ? laserPos[1][i] += laserSpeed : laserPos[0][i] = 0;
        }
    }

    // ------------------------------------------------------------------------- //
    // Functionality Section

    // Collision Detector
    float closeY[2];
    closeY[0] = rockPos[1][0];
    closeY[1] = 0;
    for (int i = 1; i < rockCount; i++)
    {
        if (closeY[0] > rockPos[1][i] - 160 && rockPos[1][i] - 160 >= 0)
        {
            closeY[0] = rockPos[1][i] - 160;
            closeY[1] = i;
        }
    }
    if ((rockPos[0][(int)closeY[1]] - rocketX >= -75 && rockPos[0][(int)closeY[1]] - rocketX <= -8) && (closeY[0] <= 2 && closeY[0] >= -10))
    {
        animState = 0;
    }

    // Score
    if (animState == 1 && prevSec != time(NULL) - startTime)
    {
        if (rocketBoosting == 0)
        {
            score += 1;
        }
        else
        {
            score += 2;
        }
        prevSec = time(NULL) - startTime;
    }
    if (prevSec2 != time(NULL) - startTime)
    {
        consoleOutput();
        prevSec2 = time(NULL) - startTime;
    }

    // Laser Hit
    for (int i = 0; i < 200; i++)
    {
        if (abs(laserPos[1][i] - rockPos[1][(int)laserPos[2][i]]) < 5 && animState == 1)
        {
            rockPos[1][(int)laserPos[2][i]] = rand() % 700 + 700;
            laserPos[0][i] = 0;
            laserPos[1][i] = 0;
            laserPos[2][i] = -1;
            score += 10;
        }
    }

    // Booster
    if (rocketBoosting == 1 && boostAvailable > 0 && animState == 1)
    {
        boostAvailable -= screenResX < 1920 ? 0.002 : 0.1;
    }
    else
    {
        resetSpeed();
        if (boostAvailable < 100 && animState == 1)
        {
            boostAvailable += screenResX < 1920 ? 0.005 : 0.15;
        }
    }

    glutPostRedisplay();
    glutSwapBuffers();
}

int main(int argc, char **argv)
{
    srand(time(0));
    // Star Random Position
    for (int i = 0; i < starCount; i++)
    {
        starPos[0][i] = rand() % 500;
        starPos[2][i] = rand() % 500;
        starPos[4][i] = rand() % 500;
        starPos[1][i] = rand() % 1400;
        starPos[3][i] = rand() % 1400;
        starPos[5][i] = rand() % 1400;
    }
    // Rocks Random Position
    for (int i = 0; i < rockCount; i++)
    {
        rockPos[0][i] = rand() % 400;
        rockPos[1][i] = (rand() % 1400) + 500;
        rockPos[2][i] = rand() % 10;
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(500, 700);
    glutCreateWindow("CG Project");
    init();
    glutKeyboardFunc(keyPressed);
    glutDisplayFunc(Draw);
    glutMainLoop();
    return 0;
}
