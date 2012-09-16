﻿//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2012-tol.          
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk. 
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat. 
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization függvényt kivéve, a lefoglalt adat korrekt felszabadítása nélkül 
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:  
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D, 
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi, 
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : RETI DAVID
// Neptun : ZNE4NN
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy 
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem. 
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a 
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb 
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem, 
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.  
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat 
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#include <math.h>
#include <stdlib.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
// MsWindows-on ez is kell
#include <windows.h>     
#endif // Win32 platform

#include <GL/gl.h>
#include <GL/glu.h>
// A GLUT-ot le kell tolteni: http://www.opengl.org/resources/libraries/glut/
#include <GL/glut.h>     


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
struct Vector {
   float x, y, z;

   Vector( ) { 
	x = y = z = 0;
   }
   Vector(float x0, float y0, float z0 = 0) { 
	x = x0; y = y0; z = z0;
   }
   Vector operator*(float a) { 
	return Vector(x * a, y * a, z * a); 
   }
   Vector operator+(const Vector& v) {
 	return Vector(x + v.x, y + v.y, z + v.z); 
   }
   Vector operator-(const Vector& v) {
 	return Vector(x - v.x, y - v.y, z - v.z); 
   }
   float operator*(const Vector& v) { 	// dot product
	return (x * v.x + y * v.y + z * v.z); 
   }
   Vector operator%(const Vector& v) { 	// cross product
	return Vector(y*v.z-z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
   }
   float Length() { return sqrt(x * x + y * y + z * z); }
};
 
//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
struct Color {
   float r, g, b;

   Color( ) { 
	r = g = b = 0;
   }
   Color(float r0, float g0, float b0) { 
	r = r0; g = g0; b = b0;
   }
   Color operator*(float a) { 
	return Color(r * a, g * a, b * a); 
   }
   Color operator*(const Color& c) { 
	return Color(r * c.r, g * c.g, b * c.b); 
   }
   Color operator+(const Color& c) {
 	return Color(r + c.r, g + c.g, b + c.b); 
   }
};

const int screenWidth = 600;	// alkalmazas ablak felbontasa
const int screenHeight = 600;

const int imageWidth = 5000;
const int imageHeight = 5000;

const float PI = atan((float)1.0)*4;

Color image[screenWidth*screenHeight];	// egy alkalmazas ablaknyi kep

//-----
// A terepet megado fuggveny
// -5000 <= x,y <= 5000 (imageWidth, imageHeight)
// 250 <= func(x,y) <= 1014
//-----
float func(float x, float y){
	return ((x*x+x*x*x*y+y*y+x*y+y)+624999975005000)/(624999975005000+625000075005000)*(1014-250)+250;
}
// df/dx és df/dy egy vektorban
Vector gradFunc(float x, float y){
	Vector t = Vector(((float)2*x+y+3*x*x*y), ((float)1+x+x*x*x+2*y));
	t = t*(191/312500012502500);
	return t;
}
//-----
//Antenna:
// -5000 <= antenna_x, antenna_y <= 5000
Vector antenna;
const float antenna_height = 20;
const char transport_antenna = 't';
//-----

//-----
//Hansel
Vector h; //pozicio
Vector hd; // irany (normalvektor)
//Gretel
Vector g; // pozicio
Vector gd; // irany (normalvektor)
//-----

//Szimulacio
const float dt = 100;
const int V = 1000; // vizszintes talajon elerheto sebesseg (m/s)
// M fokban!
float vFromM(float M){
	return V*(1-M/90);
}

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) { 
	glViewport(0, 0, screenWidth, screenHeight);

	/*for(int i = -imageWidth; i < imageWidth; i++){
		for(int j = -imageHeight; j < imageHeight; j++){
			int x = (float)(i + imageWidth) / (2*imageWidth) * screenWidth;
			int y = (float)(j + imageHeight) / (2*imageHeight) * screenHeight;
			float rgb = (func(i,j) - 250)/(1014-250);
			image[y*screenWidth + x] = Color(rgb,rgb,rgb);
		}
	}*/

	for(int i = 0; i < screenWidth; i++){
		for(int j = 0; j < screenHeight; j++){
			float x = (float)2*imageWidth/screenWidth * i - imageWidth;
			float y = (float)2*imageHeight/screenHeight * j - imageHeight;
			float rgb = (func(x,y) - 250)/(1014-250);
			image[(int)j*screenWidth + int(i)] = Color(rgb,rgb,rgb);
		}
	}

	srand(glutGet(GLUT_ELAPSED_TIME)); // Ekkor mar inicializalt a GLUT

	antenna.x = rand() % 10000 - 5000;
	antenna.y = rand() % 10000 - 5000;
	antenna.z = func(antenna.x, antenna.y) + antenna_height;

	h.x = rand() % 10000 - 5000;
	h.y = rand() % 10000 - 5000;
	h.z = func(h.x, h.y);

	g.x = rand() % 10000 - 5000;
	g.y = rand() % 10000 - 5000;
	g.z = func(g.x, g.y);

}
// Terero van-e
bool reception = false;
float total_time = 1; // osszes ido
float mobile_time = 0; // mobilozassal eltolthetett ido
// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

    // Atmasoljuk a kepet a rasztertarba
    glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, image);

	const float delta = 0.01;
	const float r = 200;
	glColor3f(0.67843f,0.84706f,0.90196f);// Hansel light blue
	glBegin(GL_LINE_STRIP);
	for(float t = 0; t < 2 * PI; t += 2 * PI * delta){
		glVertex2f((h.x + r * cos(t))/imageWidth, (h.y + r * sin(t))/imageHeight);
	}
	glEnd();
	//Mars
	float mars = PI / 4;
	glBegin(GL_LINES);
		glVertex2f((h.x + r * cos(mars))/imageWidth, (h.y + r * sin(mars))/imageHeight);
		glVertex2f((h.x + 2 * r * cos(mars))/imageWidth, (h.y + 2 * r * sin(mars))/imageHeight);
	glEnd();
	glBegin(GL_LINES);
		glVertex2f((h.x + 1.5 * r * cos(mars + PI/16))/imageWidth, (h.y + 1.5 * r * sin(mars + PI/16))/imageHeight);
		glVertex2f((h.x + 2 * r * cos(mars))/imageWidth, (h.y + 2 * r * sin(mars))/imageHeight);
	glEnd();
	glBegin(GL_LINES);
		glVertex2f((h.x + 1.5 *r * cos(mars - PI/16))/imageWidth, (h.y + 1.5 * r * sin(mars - PI/16))/imageHeight);
		glVertex2f((h.x + 2 * r * cos(mars))/imageWidth, (h.y + 2 * r * sin(mars))/imageHeight);
	glEnd();

	glColor3f(1.0f,0.75294f,0.79608f);// Gretel pink
	glBegin(GL_LINE_STRIP);
	for(float t = 0; t < 2 * PI; t += 2 * PI * delta){
		glVertex2f((g.x + r * cos(t))/imageWidth, (g.y + r * sin(t))/imageHeight);
	}
	glEnd();
	//Venus
	float venus = -PI / 2;
	glBegin(GL_LINES);
		glVertex2f((g.x + r * cos(venus))/imageWidth, (g.y + r * sin(venus))/imageHeight);
		glVertex2f((g.x + 2 * r * cos(venus))/imageWidth, (g.y + 2 * r * sin(venus))/imageHeight);
	glEnd();
	glBegin(GL_LINES);
		glVertex2f((g.x + 1.7 * r * cos(venus - PI/14))/imageWidth, (g.y + 1.7 * r * sin(venus - PI/14))/imageHeight);
		glVertex2f((g.x + 1.7 * r * cos(venus + PI/14))/imageWidth, (g.y + 1.7 * r * sin(venus + PI/14))/imageHeight);
	glEnd();

	//Pontokkent jelenitjuk meg
	//glPointSize(10.0f);
	//glColor3f(1.0f,0.0f,0.0f);// Hansel RED
	//glBegin(GL_POINTS);
	//	glVertex2f(h.x/imageWidth, h.y/imageHeight);		
	//glEnd();

	//glColor3f(0.0f,1.0f,0.0f);
	//glBegin(GL_POINTS);		// Gretel GREEN
	//	glVertex2f(g.x/imageWidth, g.y/imageHeight);		
	//glEnd();

	// Kirajzoljuk a haromszoget az antenna helyere
	glColor3f(1.0f,1.0f,0.0f);// Antenna YELLOW
	glBegin(GL_TRIANGLES);
		glVertex2f((antenna.x-100)/imageWidth, (antenna.y-100/3*sqrt(3.0))/imageHeight); 
		glVertex2f((antenna.x+100)/imageWidth, (antenna.y-100/3*sqrt(3.0))/imageHeight); 
		glVertex2f(antenna.x/imageWidth, (antenna.y+150*sqrt(3.0))/imageHeight); 
	glEnd();

	if(reception){ // ha van terero rajzolunk haromszoget
		glColor3f(1.0f,0.0f,0.0f);
		glBegin(GL_TRIANGLES);
			glVertex2f(h.x/imageWidth, h.y/imageHeight);
			glVertex2f(g.x/imageWidth, g.y/imageHeight);
			glVertex2f(antenna.x/imageWidth, antenna.y/imageHeight);
		glEnd();
	}
	//Ket teglalapot rajzolunk ami progress barkent szolgal a terero-idore
	float width = 0.05f;
	glColor3f(1.0f,0.7529f,0.0f); // Orange
	glBegin(GL_QUADS);
		glVertex2f(-1.0f+width,1.0f-width);
		glVertex2f(-1.0f+width,1.0f-3*width);		
		glVertex2f(mobile_time/total_time*(2-2*width)-1+width,1.0f-3*width);
		glVertex2f(mobile_time/total_time*(2-2*width)-1+width,1.0f-width);
	glEnd();
	glColor3f(0.0f,0.0f,0.0f); // Black
	glBegin(GL_QUADS);
		glVertex2f(+1.0f-width,1.0f-width);
		glVertex2f(+1.0f-width,1.0f-3*width);		
		glVertex2f(mobile_time/total_time*(2-2*width)-1+width,1.0f-3*width);
		glVertex2f(mobile_time/total_time*(2-2*width)-1+width,1.0f-width);
	glEnd();


    glutSwapBuffers();     				// Buffercsere: rajzolas vege

}

// Billentyuzet esemenyeket lekezelo fuggveny
void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'd') glutPostRedisplay( ); 		// d beture rajzold ujra a kepet
	if (key == transport_antenna){
		antenna.x = rand() % 10000 - 5000;
		antenna.y = rand() % 10000 - 5000;
		antenna.z = func(antenna.x, antenna.y) + antenna_height;
		glutPostRedisplay( );
	}
}

const float delta = 0.01;

void CheckReception(float dt){
	//Hansel reception
	bool hr = true;
	for(float t = 0; t < 1; t += delta){
		float x = h.x * t + antenna.x * (1-t);
		float y = h.y * t + antenna.y * (1-t);
		float z = h.z * t + antenna.z * (1-t);
		if(func(x,y) > z){
			hr = false;
			break;
		}
	}
	//Gretel reception
	bool gr = true;
	for(float t = 0; t < 1; t += delta){
		float x = g.x * t + antenna.x * (1-t);
		float y = g.y * t + antenna.y * (1-t);
		float z = g.z * t + antenna.z * (1-t);
		if(func(x,y) > z){
			gr = false;
			break;
		}
	}
	if(hr == true && gr == true){
		mobile_time += dt;
		reception = true;
	}
	else
		reception = false;
}

// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x_, int y_) {
	// Koordinatatranszformacio:
	// (0,0) -----> (screenWidth, 0)								^		(imageWidth, imageHeight)
	//   |															|
	//   |							-->								|
	//   |										--------------------|--------------------->
	//   \/															|
	//(0,screenHeight)				(-imageWidth, -imageHeight)		|
	y_ = screenHeight - y_;
	float x = 2 * imageWidth / screenWidth * x_ - imageWidth;
	float y = 2 * imageHeight / screenHeight * y_ - imageHeight;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){   // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
		Vector t = Vector(x,y) - h;
		t.z = 0;
		hd = t*(1/t.Length()); // normaljuk
		glutPostRedisplay( ); 						 // Ilyenkor rajzold ujra a kepet
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){   // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
		Vector t = Vector(x,y) - g;
		t.z = 0;
		gd = t*(1/t.Length()); // normaljuk
		glutPostRedisplay( ); 						 // Ilyenkor rajzold ujra a kepet
	}
}
void MovePeople(float dt){
	dt /= 1000;
	//Meredeksegek:
	float hM = (180/PI) * atan( gradFunc(h.x, h.y) * hd ); // grad f * v ahol v egysegvektor
	float gM = (180/PI) * atan( gradFunc(g.x, g.y) * gd );

	//Sebessegek:
	float hv = vFromM( hM );
	float gv = vFromM( gM );

	//Visszaalakitjuk radianba:
	hM = (PI/180) * hM;
	gM = (PI/180) * gM;

	// dt ido alatt v*dt-vel kerul arebb amibol az xy-sikra v*dt*cos(M) esik
	h.x = h.x + hd.x * hv * dt * cos(hM);
	h.y = h.y + hd.y * hv * dt * cos(hM);
	g.x = g.x + gd.x * gv * dt * cos(gM);
	g.y = g.y + gd.y * gv * dt * cos(gM);

	h.z = func(h.x, h.y);
	g.z = func(g.x, g.y);

	//Visszapattanunk a falakrol:
	if(h.x > imageWidth || h.x < -imageWidth)
		hd.x = -hd.x;
	if(h.y > imageHeight || h.y < -imageHeight)
		hd.y = -hd.y;
	if(g.x > imageWidth || g.x < -imageWidth)
		gd.x = -gd.x;
	if(g.y > imageHeight || g.y < -imageHeight)
		gd.y = -gd.y;
	
	if(h.x > 2*imageWidth || h.x < -2*imageWidth || h.y > 2*imageHeight || h.y < -2*imageHeight || g.x > 2*imageWidth || g.x < -2*imageWidth || g.y > 2*imageHeight || g.y < -2*imageHeight)
		exit(1);
}
void SimulateWorld(float tstart, float tend) {
	for(float ts = tstart; ts < tend; ts += dt) {
		float te = min(tend, ts + dt);
		if(ts <= total_time && te >= total_time){
			CheckReception(te-ts);
			MovePeople(te-ts);
			glutPostRedisplay();
		}
		else{
			for( int i = 0; i< 500; i++)
				i++;
		}
		total_time += dt;
		//for each object obj: obj.Control(ts, te);
		//for each object obj: obj.Animate(ts, te);
	}
}
//float time;
// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle( ) {
	float old_time = total_time;
	/*float old_time = time;*/
	float total_time = glutGet(GLUT_ELAPSED_TIME);		// program inditasa ota eltelt ido
	SimulateWorld(old_time, total_time);
}



// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
    glutInit(&argc, argv); 				// GLUT inicializalasa
    glutInitWindowSize(600, 600);			// Alkalmazas ablak kezdeti merete 600x600 pixel 
    glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer

    glutCreateWindow("Grafika hazi feladat");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon

    glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();

    onInitialization();					// Az altalad irt inicializalast lefuttatjuk

    glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
    glutMouseFunc(onMouse); 
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);

    glutMainLoop();					// Esemenykezelo hurok
    
    return 0;
}

