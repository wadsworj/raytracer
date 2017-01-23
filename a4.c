

#include "a4.h"

int SCREEN_WIDTH = 1024;
int SCREEN_HEIGHT = 768;
int NUM_SPHERES = 0;

float * pixels;

struct Sphere ** sphere;

float light[6];
float pointOfView[3] = {0, 0, 0}; // viewpoint position
float rayDir[3];   // used for finding ray-sphere intersection
float diffuse = 0.9;
float ambient = 0.1;
float specular = 1.1;


float normalizeVector(float * vector)
{
    float mag = sqrtf(vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2]);

    //printf("mag: %f\n", mag);

    return mag;
}

/* function to perform the dot product operation */
float dotProduct(float * v1, float * v2)
{
    float result = 0;
    result += (v1[0] * v2[0]);
    result += (v1[1] * v2[1]);
    result += (v1[2] * v2[2]);

    return result;
}

/* function to determine if the sphere intersects with
 * a ray */
float sphere_intersect(float sphereCenter[3], float radius, float * rayOrigin)
{
    float a, b, c, rootD, root;
    float determinant;
    float length;

    //printf("Is the ray normalized: %f\n", rayDir[0] + rayDir[1] + rayDir[2]);

    // a = x^2 + y^2 + z^2 = 1
    //a = rayDir[0]*rayDir[0] + rayDir[1]*rayDir[1] + rayDir[2]*rayDir[2];
    a = 1;

    // b = 2(x(pointOfView_x - sphereCenter_x))
    b = 2*(rayDir[0]*(rayOrigin[0] - sphereCenter[0]) + rayDir[1]*(rayOrigin[1] - sphereCenter[1]) + rayDir[2]*(rayOrigin[2]-sphereCenter[2]));
    c = (rayOrigin[0]-sphereCenter[0])*(rayOrigin[0]-sphereCenter[0]) + (rayOrigin[1]-sphereCenter[1])*(rayOrigin[1]-sphereCenter[1]) + (rayOrigin[2]-sphereCenter[2])*(rayOrigin[2]-sphereCenter[2]) - radius*radius;

    determinant = b*b - 4*a*c;

    //printf("determinant: %f\n", determinant);
    //getchar();

    /* No intersect */
    if(determinant < 0) {
        return -1;
    }

    rootD = sqrtf(determinant);

    //printf("rootD: %f\n", rootD);

    if(b < 0){
        root = (-b - rootD)/(2.0*a);
    } else {
        root = (-b + rootD)/(2.0*a);
    }


    if(root >= 0.0 /*&& root <= 1.0*/){
        //printf("root: %f\n", root);
        return root;
    } else {
        return 1.0;
    }
}

float lengthOfVector(float * vector)
{
    return sqrtf(vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2]);
}

void performRayTrace(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int count = 0;

    int i, j, k, h;
    int sphereIndex = 0;
    float x, y, z;
    float inter, tempInter;
    float norm;

    float specularValue;
    float factor;
    float rayCenter[3]; // the ray that goes from the origin to the center of the sphere


    pixels = malloc(sizeof(float)*SCREEN_WIDTH*SCREEN_HEIGHT*3);

    /* loop through each pixel in viewpoint screen */
    for (i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (j = 0; j < SCREEN_WIDTH; j++)
        {

            inter = -1;
            sphereIndex = -1;
            x = j-384;  // half of height
            //x = i-512;  // half of width
            // y = j-384;  // half of height
            y = i-512;  // half of width
            z = 800;   // distance from viewpoint to plane


            rayDir[0] = x - pointOfView[0];
            rayDir[1] = y - pointOfView[1];
            rayDir[2] = z - pointOfView[2];

            norm = normalizeVector(rayDir);
            rayDir[0] = rayDir[0]/norm; rayDir[1] = rayDir[1]/norm; rayDir[2] = rayDir[2]/norm;

            //printf("rayDir[0]: %f, rayDir[0]: %f, rayDir[0]: %f\n", rayDir[0], rayDir[1], rayDir[2]);
            //getchar();

            for (k = 0; k < NUM_SPHERES; k++)
            {

                //printf("sphere[k]->radius: %f\n", sphere[k]->radius);
                tempInter = sphere_intersect(sphere[k]->center, sphere[k]->radius, pointOfView);

                if (tempInter != -1)
                {
                    if (inter == -1)
                    {
                        inter = tempInter;
                        sphereIndex = k;
                    } else if (tempInter < inter) {
                        inter = tempInter;
                        sphereIndex = k;
                    } else {

                    }
                    
                }

            }

            /* calculate ray between object and light */
            if(sphereIndex != -1)
            {
                float dotResult;
                float pc[3];
                float puv[3];
                float b[3];
                float c;
                float temp;
                float point[3];
                float pointCenter[3];
                float normal[3];
                float reflection[3];
                
                //printf("rayDir[0]: %f, rayDir[1]: %f, rayDir[2]: %f\n", rayDir[0], rayDir[1], rayDir[2]);

                

                /* determine the ray that goes from point of view to center of sphere */
                rayCenter[0] = sphere[sphereIndex]->center[0] - pointOfView[0];
                rayCenter[1] = sphere[sphereIndex]->center[1] - pointOfView[1];
                rayCenter[2] = sphere[sphereIndex]->center[2] - pointOfView[2];

                

                /* project the center of the sphere onto rayDir */
                temp = dotProduct(rayDir, rayCenter);
                temp = temp/lengthOfVector(rayDir);

                //printf("temp (dotProduct): %f\n", temp);

                //temp = temp/(lengthOfVector(rayDir));

                //printf("temp: %f\n", temp);

                puv[0] = rayDir[0] * temp;
                puv[1] = rayDir[1] * temp;
                puv[2] = rayDir[2] * temp;


                //printf("puv[0]: %f, puv[1]: %f, puv[2]: %f\n", puv[0], puv[1], puv[2]);
                //printf("pointOfView[0]: %f, pointOfView[1]: %f, pointOfView[2]: %f\n", pointOfView[0], pointOfView[1], pointOfView[2]);

                /* b = | pc - c | */
                b[0] = puv[0] - rayCenter[0];
                b[1] = puv[1] - rayCenter[1];
                b[2] = puv[2] - rayCenter[2];

                temp = lengthOfVector(b);

                // printf("temp (length): %f\n", temp);

                //printf("sphere[sphereIndex]->radius*sphere[sphereIndex]->radius) - (temp * temp): %f\n", (sphere[sphereIndex]->radius*sphere[sphereIndex]->radius) - (temp * temp));
                
                // c^2 = a^2 - b^2
                c = sqrtf((sphere[sphereIndex]->radius*sphere[sphereIndex]->radius) - (temp * temp));


                // di1 = | pc - p | - c
                point[0] = puv[0] - pointOfView[0];
                point[1] = puv[1] - pointOfView[1];
                point[2] = puv[2] - pointOfView[2];

                /* length of distance from viewpoint to point of intersection
                    di1 = |pc - p| - c */
                temp = lengthOfVector(point);

                //printf("point[0]: %f, point[1]: %f, point[2]: %f\n", point[0], point[1], point[2]);
                //printf("temp: %f, c: %f\n", temp, c);

                temp = temp - c;

                // i1 = p + d * di1
                point[0] = pointOfView[0] + rayDir[0]*temp;
                point[1] = pointOfView[1] + rayDir[1]*temp;
                point[2] = pointOfView[2] + rayDir[2]*temp;

                /* determine the vector point to light */
                puv[0] = light[0] - point[0];
                puv[1] = light[1] - point[1];
                puv[2] = light[2] - point[2];

                norm = normalizeVector(puv);
                puv[0] = puv[0]/norm; puv[1] = puv[1]/norm; puv[2] = puv[2]/norm;


                /* Calculate Reflection */
                reflection[0] = 2*(dotProduct(normal, puv))*normal[0] - puv[0];
                reflection[1] = 2*(dotProduct(normal, puv))*normal[1] - puv[1];
                reflection[2] = 2*(dotProduct(normal, puv))*normal[2] - puv[2];

                specularValue = specular*(dotProduct(reflection, rayDir));

                specularValue = powf(specularValue, 10);

                /* Calculate Unit Normal Vector */
                normal[0] = (point[0] - sphere[sphereIndex]->center[0])/sphere[sphereIndex]->radius;
                normal[1] = (point[1] - sphere[sphereIndex]->center[1])/sphere[sphereIndex]->radius;
                normal[2] = (point[2] - sphere[sphereIndex]->center[2])/sphere[sphereIndex]->radius;

                norm = normalizeVector(normal);
                normal[0] = normal[0]/norm; normal[1] = normal[1]/norm; normal[2] = normal[2]/norm;


                rayDir[0] = puv[0];
                rayDir[1] = puv[1];
                rayDir[2] = puv[2];

                factor = dotProduct(normal, rayDir);



                for (h = 0; h < NUM_SPHERES; h++)
                {

                    //printf("dotProduct: %f\n", dotProduct(pointCenter, rayDir));
                    //getchar();

                    /* calculate distance for each intersection */
                    if( ((temp = sphere_intersect(sphere[h]->center, sphere[h]->radius, point)) != 1.0) && (temp > 0.9) )
                    {
                        pixels[count] = ambient*sphere[sphereIndex]->colour[0];
                        pixels[count + 1] = ambient*sphere[sphereIndex]->colour[1];
                        pixels[count + 2] = ambient*sphere[sphereIndex]->colour[2];
                        factor = 0;
                        specularValue = 0;
                        //sphereIndex = -2;
                        break;
                    }

                }

                //dotResult = dotProduct(rayDir, rayCenter);

            }



            //printf("sphereIndex: %d\n", sphereIndex);
            //getchar();

            if (sphereIndex == -2) {

            } else if(sphereIndex != -1)
            {
                /* draw the pixel */
                //printf("sphereIndex: %d\n", sphereIndex);
                //printf("r: %f, g: %f, b: %f\n", sphere[sphereIndex]->colour[0], sphere[sphereIndex]->colour[1], sphere[sphereIndex]->colour[2]);
                //getchar();
                pixels[count] = light[3]*(diffuse*factor*sphere[sphereIndex]->colour[0] + specular*specularValue*sphere[sphereIndex]->colour[0]) + ambient*sphere[sphereIndex]->colour[0];
                pixels[count + 1] = light[4]*(diffuse*factor*sphere[sphereIndex]->colour[1] + specular*specularValue*sphere[sphereIndex]->colour[1]) + ambient*sphere[sphereIndex]->colour[1];
                pixels[count + 2] = light[5]*(diffuse*factor*sphere[sphereIndex]->colour[2] + specular*specularValue*sphere[sphereIndex]->colour[2]) + ambient*sphere[sphereIndex]->colour[2];



                //glFlush();
            } else {
                pixels[count] = 0.0;
                pixels[count + 1] = 0.0;
                pixels[count + 2] = 0.0;
            }

            count += 3;
        }
    }

    glDrawPixels(SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_FLOAT, pixels);

    printf("count: %d\n", count);

    glFlush();

}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    performRayTrace();
    glFlush();
}


void init(void)
{
    glClearColor (0.0, 0.0, 0.0, 1.0);
    glOrtho(-454.0, 454.0, -250.0, 250.0, -250.0, 250.0);
    glEnable(GL_DEPTH_TEST);
}


void loadObjectFile(const char * filename)
{
    FILE * fp;
    int i = 0;
    int c;
    fp = fopen(filename, "r");

    /* space for 1000 spheres */
    sphere = malloc(sizeof(struct Sphere *)*1000);

    float x, y, z;
    float r, g, b;
    float radius;

    while ((c = fgetc(fp)) != ' ')
    {
        printf("c: %c\n", c);
    }

    ungetc(c, fp);
    fscanf(fp, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);

    light[0] = x;
    light[1] = y;
    light[2] = z;
    light[3] = r;
    light[4] = g;
    light[5] = b;

    while ((c = fgetc(fp)) != -1)
    {
        printf("c: %c\n", c);
        /* consume word */
        while ((c = fgetc(fp)) != ' ')
        {

        }
        ungetc(c, fp);

        fscanf(fp, "%f %f %f %f %f %f %f", &x, &y, &z, &radius, &r, &g, &b);

        sphere[i] = malloc(sizeof(struct Sphere));
        

        sphere[i]->center[0] = x;
        sphere[i]->center[1] = y;
        sphere[i]->center[2] = z;

        sphere[i]->radius = radius;

        sphere[i]->colour[0] = r;
        sphere[i]->colour[1] = g;
        sphere[i]->colour[2] = b;

        printf("x: %f, y: %f, z: %f, radius: %f, r: %f, g: %f, b: %f\n", x, y, z, radius, r, g, b);

        i++;

        printf("i: %d\n", i);
    }

    NUM_SPHERES = i;

    printf("NUM_SPHERES: %d\n", NUM_SPHERES);

}

int main(int argc, char ** argv)
{
    loadObjectFile(argv[1]);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("A4 - Raytacing");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}