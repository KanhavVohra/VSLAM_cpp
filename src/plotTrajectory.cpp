#include "plotTrajectory.h"


int RunPangolinTest() {
    // Create a window and bind its OpenGL context
    pangolin::CreateWindowAndBind("Pangolin OpenGL Test", 640, 480);

    // Enable depth testing (not strictly needed for a triangle, but common)
    glEnable(GL_DEPTH_TEST);

    // Main loop
    while (!pangolin::ShouldQuit()) {
        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Simple immediate-mode triangle (fine for a quick test)
        glBegin(GL_TRIANGLES);
            glVertex2f(-0.5f, -0.5f);
            glVertex2f( 0.5f, -0.5f);
            glVertex2f( 0.0f,  0.5f);
        glEnd();

        // Swap buffers + process events
        pangolin::FinishFrame();
    }

    return 0;
}


//#################/// 

int testPangolin() {
    pangolin::CreateWindowAndBind("Pangolin Test", 640, 480);
    glEnable(GL_DEPTH_TEST);


    while (!pangolin::ShouldQuit()) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        pangolin::FinishFrame();
    }

    return 0;
}




int myPangolin() {
    pangolin::CreateWindowAndBind("MYPANGOLIN",800,800);
    glEnable(GL_DEPTH_TEST);

    return 0;
}