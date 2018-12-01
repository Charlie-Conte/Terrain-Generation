#include "main.h"

// Function to read text file, used to read shader files
char* readTextFile(char* aTextFile)
{
	FILE* filePointer = fopen(aTextFile, "rb");
	char* content = NULL;
	long numVal = 0;

	fseek(filePointer, 0L, SEEK_END);
	numVal = ftell(filePointer);
	fseek(filePointer, 0L, SEEK_SET);
	content = (char*)malloc((numVal + 1) * sizeof(char));
	fread(content, 1, numVal, filePointer);
	content[numVal] = '\0';
	fclose(filePointer);
	return content;
}



// Initialization routine.
void setup(void)
{
	// Initialise terrain - set values in the height map to 0
	float terrain[MAP_SIZE][MAP_SIZE] = {};
	for (int x = 0; x < MAP_SIZE; x++)
	{
		for (int z = 0; z < MAP_SIZE; z++)
		{
			terrain[x][z] = 0;
		}
	}


	int itteration = 1;
	int stepSize = MAP_SIZE - 1;
	float randomMax = 10;
	float roughness = .2;

	terrain[0][0] = (linearRand(-randomMax, randomMax) * 2 * roughness) - roughness;
	terrain[0][MAP_SIZE - 1] = (linearRand(-randomMax, randomMax) * 2 * roughness) - roughness;
	terrain[MAP_SIZE - 1][0] = (linearRand(-randomMax, randomMax) * 2 * roughness) - roughness;
	terrain[MAP_SIZE - 1][MAP_SIZE - 1] = (linearRand(-randomMax, randomMax) * 2 * roughness) - roughness;


	cout << "Start\n";
	while (stepSize >= 2.0f)
	{
		int hStepSize = stepSize / 2;
		cout << endl << "Itteration: " << itteration << endl;
		for (int x = 0; x < MAP_SIZE - 1; x += stepSize)
		{
			for (int y = 0; y < MAP_SIZE - 1; y += stepSize)
			{
				float avgHeight = ((
					terrain[x][y] +
					terrain[x + stepSize][y] +
					terrain[x][y + stepSize] +
					terrain[x + stepSize][y + stepSize]
					) / 4.0f) + (linearRand(0.0f, randomMax) * 2 * roughness) - roughness;
				terrain[x + hStepSize][y + hStepSize] = avgHeight + linearRand(-randomMax, randomMax);
				//cout << "(" << x << "," << y << ")\n";
			}
		}


		for (int x = 0; x < MAP_SIZE - 1; x += hStepSize)
		{
			for (int y = (x + hStepSize) % stepSize; y < MAP_SIZE - 1; y += stepSize)
			{
				int sMO = MAP_SIZE - 1;
				float avgHeightRan =
					((
					terrain[(x - hStepSize + MAP_SIZE - 1) % sMO][y] +
					terrain[(x + hStepSize) % sMO][y] +
					terrain[x][(y - hStepSize + MAP_SIZE - 1) % sMO] +
					terrain[x][(y + hStepSize) % sMO]
					) / 4.0f) + (linearRand(-randomMax, randomMax) * 2 * roughness) - roughness;
				terrain[x][y] = avgHeightRan;
				if (x == 0) terrain[MAP_SIZE - 1][y] = avgHeightRan;
				if (y == 0) terrain[x][MAP_SIZE - 1] = avgHeightRan;
				//		cout << "(" << x << "," << y << ")\t\t" << "Height: " << avgHeightRan << endl;
			}
		}



		randomMax = randomMax / 2;
		stepSize = stepSize / 2;
		itteration++;
	}



	// Intialise vertex array
	int i = 0;

	for (int z = 0; z < MAP_SIZE; z++)
	{
		for (int x = 0; x < MAP_SIZE; x++)
		{
			// Set the coords (1st 4 elements) and a default colour of black (2nd 4 elements) 
			terrainVertices[i] = { { (MAP_SIZE-1.0f) / 2.0f - (float)x, terrain[x][z], (MAP_SIZE - 1.0f) / 2.0f - (float)z, 1.0 },{ 0.0, 0.0, 0.0, 1.0 } };
			i++;
		}
	}
	//int midVert = (int)size(terrainVertices) / 2;
	//cout << midVert << endl;
	//cout << terrainVertices[midVert].coords[0] << ",\t" << terrainVertices[midVert].coords[1]<<",\t" << terrainVertices[midVert].coords[2] << endl;
	// Now build the index terrain 
	i = 0;
	for (int z = 0; z < MAP_SIZE - 1; z++)
	{
		i = z * MAP_SIZE;
		for (int x = 0; x < MAP_SIZE * 2; x += 2)
		{
			terrainIndexData[z][x] = i;
			i++;
		}
		for (int x = 1; x < MAP_SIZE * 2 + 1; x += 2)
		{
			terrainIndexData[z][x] = i;
			i++;
		}
	}


	glClearColor(1.0, 1.0, 1.0, 0.0);

	// Create shader program executable - read, compile and link shaders
	char* vertexShader = readTextFile("vertexShader.glsl");
	vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, (const char**)&vertexShader, NULL);
	glCompileShader(vertexShaderId);

	char* fragmentShader = readTextFile("fragmentShader.glsl");
	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, (const char**)&fragmentShader, NULL);
	glCompileShader(fragmentShaderId);

	programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);
	glUseProgram(programId);
	///////////////////////////////////////

	// Create vertex array object (VAO) and vertex buffer object (VBO) and associate terrain with vertex shader.
	glGenVertexArrays(1, vao);
	glGenBuffers(1, buffer);
	glBindVertexArray(vao[TERRAIN]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[TERRAIN_VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(terrainVertices), terrainVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(terrainVertices[0]), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(terrainVertices[0]), (GLvoid*)sizeof(terrainVertices[0].coords));
	glEnableVertexAttribArray(1);
	///////////////////////////////////////

	// Obtain projection matrix uniform location and set value.
	projMatLoc = glGetUniformLocation(programId, "projMat");
	projMat = perspective(radians(60.0), (double)SCREEN_WIDTH / (double)SCREEN_HEIGHT, 0.1, 10000.0);
	glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));

	///////////////////////////////////////

	// Obtain modelview matrix uniform location and set value.
	mat4 modelViewMat = mat4(1.0);
	modelViewMatLoc = glGetUniformLocation(programId, "modelViewMat");
	glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));

	///////////////////////////////////////
	calculateView();
}

// Drawing routine.
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	// Obtain modelview matrix uniform location and set value.
	mat4 modelViewMat = mat4(1.0);
	// Move terrain into view - glm::translate replaces glTranslatef
	modelViewMat = lookAt(eye, los, vec3(0, 1, 0)); //translate(modelViewMat, vec3(-15.0f, -12.0f, -60)); // 5x5 grid



	glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));


	// For each row - draw the triangle strip
	for (int i = 0; i < MAP_SIZE - 1; i++)
	{
		glDrawElements(GL_TRIANGLE_STRIP, verticesPerStrip, GL_UNSIGNED_INT, terrainIndexData[i]);
	}

	glFlush();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
	case 'd':
		if (key == 'a')
		{
			if (los.x >= MAP_SIZE - speed)break;
			los.x += speed * MAP_SIZE;

		}
		else
		{
			if (los.x <= -MAP_SIZE + speed)break;
			los.x -= speed * MAP_SIZE;
		}
		glutPostRedisplay();
		break;	
	case 'w':
	case 's':
		if (key == 'w')
		{
			if (los.z >= MAP_SIZE - speed)break;
			los.z += speed * MAP_SIZE;

		}
		else
		{
			if (los.z <= -MAP_SIZE + speed)break;
			los.z -= speed * MAP_SIZE;
		}
		glutPostRedisplay();
		break;
	case 27:
		exit(0);
		break;

	default:
		break;
	}
}
// Keyboard input processing routine.
void specialKeyInput(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_RIGHT:
	case GLUT_KEY_LEFT:
		if (key == GLUT_KEY_RIGHT)
		{
			camTheta -= speed;
		}
		else
		{
			camTheta += speed;
		}

		calculateView();

		//los.x = cosf(camPhi) * sinf(camTheta);
		//los.y = sinf(camPhi);
		//los.z = cosf(camPhi) * -cosf(camTheta);
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN:
		if (key == GLUT_KEY_DOWN)
		{
			camPhi -= speed;
		}
		else
		{
			camPhi += speed;
		}
		//
		calculateView();
		//los.x = cosf(camPhi) * sinf(camTheta);
		//los.y = sinf(camPhi);
		//los.z = cosf(camPhi) * -cosf(camTheta);
		glutPostRedisplay();
		break;

	default:
		break;
	}
	//cout << los.x << ",\t" << los.y << ",\t" << los.z<< endl;
	cout << eye.x << ",\t" << eye.y << ",\t" << eye.z<< endl;
	cout << camPhi << endl;
}
void calculateView()
{
	eye.x = zoom * cos(camPhi) * sin(camTheta);
	eye.y = zoom * sin(camPhi);
	eye.z = zoom * cos(camPhi) * -cos(camTheta);

	//eye.x = zoom * cosf(camTheta);
	////eye.y = 0;
	//eye.z = zoom * sinf(camTheta);
	//los.x = -zoom * cosf(camTheta);
	////los.y = 0;
	//los.z = -zoom * sinf(camTheta);
}
void mouseKeyInput(int button, int state, int x, int y)
{
	switch (button)
	{

	case 3://wheel up
	case 4://wheel down
		if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
		if (button == 4)
		{
			zoom = zoom / (speed * 4);
		}
		else
		{
			zoom = zoom * speed * 4;
		}
		cout << zoom << endl;
		//cout << "ex: " << eye.x << "\tez: " << eye.z << endl;
		calculateView();
		glutPostRedisplay();
		break;

	}

}


// Main routine.
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	// Set the version of OpenGL (4.2)
	glutInitContextVersion(4, 2);
	// The core profile excludes all discarded features
	glutInitContextProfile(GLUT_CORE_PROFILE);
	// Forward compatibility excludes features marked for deprecation ensuring compatability with future versions
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("TerrainGeneration");

	// Set OpenGL to render in wireframe mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutSpecialFunc(specialKeyInput);
	glutMouseFunc(mouseKeyInput);


	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}
