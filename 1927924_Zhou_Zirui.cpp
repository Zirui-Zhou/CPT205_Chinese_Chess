#define FREEGLUT_STATIC
#include <GL/freeglut.h>
#include <iostream>

#define MAX_CHAR 128
using namespace std;


float fltFOV = 70;		// Field Of View.
float fltZoom = 1.0;	// Zoom amount.
float fltRange = 500;	// Camera position in x-axis and z-axis.
float fltY0 = 500.0;	// Camera position in y-axis.
float fltXRef = 0.0;	// Look At reference point.
float fltYRef = 0.0;
float fltZRef = 0.0;
float fltXUp = 0.0;		// Up vector.
float fltYUp = 1.0;
float fltZUp = 0.0;
float fltAngle = 0;		// Used for rotating camera.

float block_size = 70;		// Length of a block.
float board_size[] = {10 * block_size, 20, 11 * block_size};	// Length of the board.

float chess_radius = 30;	// Radius of a chess.
float chess_height = 30;	// Height of a chess.
float chess_half_height = chess_height / 2;		// Half height of a chess
float chess_top_radius = sqrt(pow(chess_radius, 2) - pow(chess_half_height, 2));	// Radius of a chess's top circle.

float y_offset = 1;			// General offset in y-axis to avoid overlapping.
float line_width = (2 - fltZoom) * 2;	// General line width (refer to zoom to avoid line clot in distance).

GLuint texture_list[2];		// List of texture id for binding.

enum chess_type {general = 0, advisor = 1, elephant = 2, horse = 3, cannon = 4, chariot = 5, soldier = 6};
enum player_type {pl_null = -1, pl_red = 0, pl_black = 1};
enum move_type {mv_block = 0, mv_self = 1, mv_space = 2, mv_enemy = 3};

struct image* wood_texture;		// Image info for wood texture.
struct image* chess_texture;	// Image info for chess texture.

class chessboard;
class chess;


// Image structure of image information and data.
struct image {
	GLint width;
	GLint height;
	GLint pixellength;
	GLubyte* pixeldata;
};


// Class for one single chess.
class chess {
public:
	int type;
	player_type player;
	int position[2];
	chessboard* chessboard;

	void move();
	void draw_chess_texture();
	void draw_chess_body();

private:
	void general_move();
	void advisor_move();
	void elephant_move();
	void horse_move();
	void cannon_move();
	void chariot_move();
	void soldier_move();
};


// Class for the whole chessboard.
class chessboard {
public:
	player_type player;
	chess* selected;

	void init_chess();
	void move_pointer(char key);
	void select();
	player_type get_chess_player(int x, int y);
	void set_move_map(int x, int y, move_type type);
	bool check_general (int aimx, int aimy);
	void draw();

private:
	chess* chess_map[9][10];
	move_type move_map[9][10];
	int pt_position[2];

	void draw_chess_body();
	void draw_chess_texture();
	void draw_single_cross_line();
	void draw_single_kill_line();
	void draw_cross_line();
	void draw_board_line();
	void draw_move_hint();
	void draw_pointer();
	void draw_board();
};


// A global chessboard object.
chessboard my_chessboard;


// Draw a string after initiating font.
void drawString(const char* str) {
	static int isFirstCall = 1;
	static GLuint lists;
	if (isFirstCall) {
		isFirstCall = 0;
		lists = glGenLists(MAX_CHAR);
		wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);
	}
	for (; *str != '\0'; ++str) {
		glCallList(lists + *str);
	}
}


// Initialize font info.
void selectFont(int size, int charset, const char* face) {
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0, charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);
}


// Draw a simple circle.
void draw_circle(double r, int n, GLenum mode = GL_FILL) {
	glPolygonMode(GL_FRONT_AND_BACK, mode);
	glBegin(GL_POLYGON);
		for (int t = 0; t <= 360; t += int(360 / n)) {
			double temp_x = r * cos(t * 3.14169 / 180);
			double temp_y = r * sin(t * 3.14169 / 180);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(temp_x, 0, temp_y);
		}
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


// Draw a simple cube.
void draw_cube(GLfloat size)
{
	static GLfloat n[6][3] =
	{
	  {-1.0, 0.0, 0.0},
	  {0.0, 1.0, 0.0},
	  {1.0, 0.0, 0.0},
	  {0.0, -1.0, 0.0},
	  {0.0, 0.0, 1.0},
	  {0.0, 0.0, -1.0}
	};
	static GLint faces[6][4] =
	{
	  {0, 1, 2, 3},
	  {3, 2, 6, 7},
	  {7, 6, 5, 4},
	  {4, 5, 1, 0},
	  {5, 6, 2, 1},
	  {7, 4, 0, 3}
	};
	GLfloat v[8][3];
	GLint i;

	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = size / 2;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = size / 2;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = size / 2;

	for (i = 5; i >= 0; i--) {
		glBegin(GL_QUADS);
		glNormal3fv(&n[i][0]);
		glTexCoord2f(0.0, 0.0);
		glVertex3fv(&v[faces[i][0]][0]);
		glTexCoord2f(1.0, 0.0);
		glVertex3fv(&v[faces[i][1]][0]);
		glTexCoord2f(1.0, 1.0);
		glVertex3fv(&v[faces[i][2]][0]);
		glTexCoord2f(0.0, 1.0);
		glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
	}
}


// Load a image into a image struct by path.
static struct image* load_image(const string path, bool has_alpha = false) {
	struct image* my_image;
	my_image = (struct image*)malloc(sizeof(struct image));
	// Read in and open an image file
	FILE* pfile = NULL; // The image file should be placed in the same folder with the source code.
	fopen_s(&pfile, path.c_str(), "rb"); // read in binary mode
	if (pfile == 0) exit(0);
	// get the width and height of image
	// reposition stream position indicator
	// SEEK_SET: Beginning of file
	fseek(pfile, 0x0012, SEEK_SET); // skip 16 bits from beginning for bmp files
	// get the width of image
	fread(&my_image->width, sizeof(my_image->width), 1, pfile);
	// get the height of image
	fread(&my_image->height, sizeof(my_image->height), 1, pfile);
	// count the length of in the image by pixel
	// pixel data consists of three colors red, green and blue (Windows implement BGR)
	my_image->pixellength = my_image->width * (has_alpha ? 4 : 3);
	// pixel data width should be an integral multiple of 4, which is required by the .bmp file
	while (my_image->pixellength % 4 != 0)
		my_image->pixellength++;
	// pixel data length = width * height
	my_image->pixellength *= my_image->height;
	// malloc for the image by pixel
	my_image->pixeldata = (GLubyte*)malloc(my_image->pixellength); // memory allocation
	if (my_image->pixeldata == 0)
		exit(0);
	// read the data of image as pixel
	fseek(pfile, 54, SEEK_SET);
	fread(my_image->pixeldata, my_image->pixellength, 1, pfile);
	// close file
	fclose(pfile);
	return my_image;
}


// A general origin translate function to locate (0, 0) in the chessboard.
void translate_origin() {
	glTranslatef(board_size[0] / 2 - block_size, y_offset, board_size[2] / 2 - block_size);
}


// A general point translate function to locate (x, y) in the chessboard.
void translate_point(int x, int y) {
	glTranslatef(-x * block_size, 0, -y * block_size);
}


// A general move function to identify the reachable position.
void chess::move() {
	switch (type) {
		case general:
			general_move();
			break;
		case advisor:
			advisor_move();
			break;
		case elephant:
			elephant_move();
			break;
		case horse:
			horse_move();
			break;
		case cannon:
			cannon_move();
			break;
		case chariot:
			chariot_move();
			break;
		case soldier:
			soldier_move();
			break;
	}
	// Set the current into a mv_self state for cancellation.
	chessboard->set_move_map(position[0], position[1], mv_self);
}


// The move function for general.
void chess::general_move() {
	int startx = position[0], starty = position[1];
	for (int aimx = 0; aimx < 9; aimx++) {
		for (int aimy = 0; aimy < 10; aimy++) {
			int tempx = aimx - startx, tempy = aimy - starty;
			player_type aimt = chessboard->get_chess_player(aimx, aimy);

			bool is_not_friend = (player != aimt);
			bool is_enemy = (aimt != pl_null) && is_not_friend;
			bool is_step = (tempy * tempy + tempx * tempx == 1);
			// General can only move in palace.
			bool is_zone = (aimx >= 3 && aimx <= 5 && aimy % 7 >= 0 && aimy % 7 <= 2);

			// General can move as chariot only when the target is also the general (flying general).
			bool is_fly = !(tempx && tempy);
			bool is_not_block = true;
			// The path cannot be blocked by any chess.
			for (int i = min(1, tempx + 1); i <= max(-1, tempx - 1); i++)
				if (chessboard->get_chess_player(startx + i, starty) != pl_null)
					is_not_block = false;
			for (int i = min(1, tempy + 1); i <= max(-1, tempy - 1); i++)
				if (chessboard->get_chess_player(startx, starty + i) != pl_null)
					is_not_block = false;
			bool is_general = chessboard->check_general(aimx, aimy);

			if ((is_step && is_zone && is_not_friend) || (is_fly && is_not_block && is_general))
				chessboard->set_move_map(aimx, aimy, is_enemy ? mv_enemy : mv_space);
			else
				chessboard->set_move_map(aimx, aimy, mv_block);
		}
	}
}


// The move function for advisor.
void chess::advisor_move() {
	int startx = position[0], starty = position[1];
	for (int aimx = 0; aimx < 9; aimx++) {
		for (int aimy = 0; aimy < 10; aimy++) {
			int tempx = aimx - startx, tempy = aimy - starty;
			player_type aimt = chessboard->get_chess_player(aimx, aimy);

			bool is_not_friend = (player != aimt);
			bool is_enemy = (aimt != pl_null) && is_not_friend;
			bool is_step = (tempy * tempy + tempx * tempx == 2);
			// Advisors can only move in palace.
			bool is_zone = (aimx >= 3 && aimx <= 5 && aimy % 7 >= 0 && aimy % 7 <= 2);

			if (is_step && is_zone && is_not_friend)
				chessboard->set_move_map(aimx, aimy, is_enemy ? mv_enemy : mv_space);
			else
				chessboard->set_move_map(aimx, aimy, mv_block);
		}
	}
}


// The move function for elephant.
void chess::elephant_move() {
	int startx = position[0], starty = position[1];
	for (int aimx = 0; aimx < 9; aimx++) {
		for (int aimy = 0; aimy < 10; aimy++) {
			int tempx = aimx - startx, tempy = aimy - starty;
			player_type aimt = chessboard->get_chess_player(aimx, aimy);

			bool is_not_friend = (player != aimt);
			bool is_enemy = (aimt != pl_null) && is_not_friend;
			bool is_step = (tempy * tempy + tempx * tempx == 8);
			// Elephants can only move in friendly zone.
			bool is_zone = (starty / 5 == aimy / 5);
			// The path cannot be blocked by any chess.
			bool is_not_block = chessboard->get_chess_player(startx+tempx/2, starty+tempy/2) == pl_null;

			if (is_step && is_zone && is_not_friend && is_not_block)
				chessboard->set_move_map(aimx, aimy, is_enemy ? mv_enemy : mv_space);
			else
				chessboard->set_move_map(aimx, aimy, mv_block);
		}
	}
}


// The move function for horse.
void chess::horse_move() {
	int startx = position[0], starty = position[1];
	for (int aimx = 0; aimx < 9; aimx++) {
		for (int aimy = 0; aimy < 10; aimy++) {
			int tempx = aimx - startx, tempy = aimy - starty;
			player_type aimt = chessboard->get_chess_player(aimx, aimy);

			bool is_not_friend = (player != aimt);
			bool is_enemy = (aimt != pl_null) && is_not_friend;
			bool is_step = (tempy * tempy + tempx * tempx == 5);
			// The path cannot be blocked by any chess.
			bool is_not_block = chessboard->get_chess_player(startx+tempx/2, starty+tempy/2) == pl_null;

			if (is_step && is_not_friend && is_not_block)
				chessboard->set_move_map(aimx, aimy, is_enemy ? mv_enemy : mv_space);
			else
				chessboard->set_move_map(aimx, aimy, mv_block);
		}
	}
}


// The move function for cannon.
void chess::cannon_move() {
	int startx = position[0], starty = position[1];
	for (int aimx = 0; aimx < 9; aimx++) {
		for (int aimy = 0; aimy < 10; aimy++) {
			int tempx = aimx - startx, tempy = aimy - starty;
			player_type aimt = chessboard->get_chess_player(aimx, aimy);

			bool is_space = aimt == pl_null;
			bool is_enemy = (!is_space) && (player != aimt);
			bool is_step = !(tempx && tempy);
			int block_num = 0;
			// The path cannot be blocked by any chess if the destination is space.
			// The path must be blocked by one single chess if the destination is enemy.
			for (int i = min(1, tempx+1); i <= max(-1, tempx-1); i++)
				if (chessboard->get_chess_player(startx+i, starty) != pl_null)
					block_num++;
			for (int i = min(1, tempy+1); i <= max(-1, tempy-1); i++)
				if (chessboard->get_chess_player(startx, starty+i) != pl_null)
					block_num++;

			if (is_step && ((is_space && block_num == 0) || (is_enemy && block_num == 1)))
				chessboard->set_move_map(aimx, aimy, is_enemy ? mv_enemy : mv_space);
			else
				chessboard->set_move_map(aimx, aimy, mv_block);
		}
	}
}


// The move function for chariot.
void chess::chariot_move() {
	int startx = position[0], starty = position[1];
	for (int aimx = 0; aimx < 9; aimx++) {
		for (int aimy = 0; aimy < 10; aimy++) {
			int tempx = aimx - startx, tempy = aimy - starty;
			player_type aimt = chessboard->get_chess_player(aimx, aimy);

			bool is_not_friend = (player != aimt);
			bool is_enemy = (aimt != pl_null) && is_not_friend;
			bool is_step = !(tempx && tempy);
			bool is_not_block = true;
			// The path cannot be blocked by any chess.
			for (int i = min(1, tempx + 1); i <= max(-1, tempx - 1); i++)
				if (chessboard->get_chess_player(startx + i, starty) != pl_null)
					is_not_block = false;
			for (int i = min(1, tempy + 1); i <= max(-1, tempy - 1); i++)
				if (chessboard->get_chess_player(startx, starty + i) != pl_null)
					is_not_block = false;

			if (is_not_friend && is_step && is_not_block)
				chessboard->set_move_map(aimx, aimy, is_enemy ? mv_enemy : mv_space);
			else
				chessboard->set_move_map(aimx, aimy, mv_block);
		}
	}
}


// The move function for soldier.
void chess::soldier_move() {
	int startx = position[0], starty = position[1];
	for (int aimx = 0; aimx < 9; aimx++) {
		for (int aimy = 0; aimy < 10; aimy++) {
			int tempx = aimx - startx, tempy = aimy - starty;
			player_type aimt = chessboard->get_chess_player(aimx, aimy);

			bool is_not_friend = (player != aimt);
			bool is_enemy = (aimt != pl_null) && is_not_friend;
			// Soldiers can only go forward in friendly zone.
			// But, they can go forward or turn left/right in enemy zone.
			// Retreat is not allowed in any time.
			bool is_forward = (tempx == 0) && (player == pl_red ? tempy == -1 : tempy == 1);
			bool is_turn = abs(tempx) <= 1 && (tempy == 0);
			bool is_zone = (player == pl_red ? starty >= 5 : starty <= 4);

			if (is_not_friend && (is_forward || (!is_zone && is_turn)))
				chessboard->set_move_map(aimx, aimy, is_enemy ? mv_enemy : mv_space);
			else
				chessboard->set_move_map(aimx, aimy, mv_block);
		}
	}
}


// Draw one chess's texture (i.e. the picture to identify).
void chess::draw_chess_texture() {
	GLdouble selected_height = 50.0f;

	glPushMatrix();
		translate_origin();
		translate_point(position[0], position[1]);
		glTranslatef(0, chess_height + y_offset * 3, 0);

		// If this chess is selected, it will be lifted.
		if (chessboard->selected == this) {
			glTranslatef(0, selected_height, 0);
		}

		glColor4f(1, 1, 1, 1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture_list[1]);
		// Enable the blend mode for tranparency rendering.
		glEnable(GL_BLEND);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
		// The basic principle is not understood.
		// Generally, it works like removing the background into transparency.
		glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_SRC_COLOR);

		// Clip the texture in one image.
		float unit_width = 1.0 / 7.0;
		float unit_height = 1.0 / 2.0;
		float origin_x = (6 - type) * unit_width;
		float origin_y = (1 - player) * unit_height;

		// Overturn the image into different viewpoints.
		if (player == pl_red) {
			glBegin(GL_QUADS);
			glTexCoord2f(1 - (origin_x), origin_y);
			glVertex3f(-chess_top_radius, 1, -chess_top_radius);
			glTexCoord2f(1 - (origin_x + unit_width), origin_y);
			glVertex3f(chess_top_radius, 1, -chess_top_radius);
			glTexCoord2f(1 - (origin_x + unit_width), origin_y + unit_height);
			glVertex3f(chess_top_radius, 1, chess_top_radius);
			glTexCoord2f(1 - (origin_x), origin_y + unit_height);
			glVertex3f(-chess_top_radius, 1, chess_top_radius);
			glEnd();
		}
		else if (player == pl_black) {
			glBegin(GL_QUADS);
			glTexCoord2f(1 - (origin_x + unit_width), origin_y + unit_height);
			glVertex3f(-chess_top_radius, 1, -chess_top_radius);
			glTexCoord2f(1 - (origin_x), origin_y + unit_height);
			glVertex3f(chess_top_radius, 1, -chess_top_radius);
			glTexCoord2f(1 - (origin_x), origin_y);
			glVertex3f(chess_top_radius, 1, chess_top_radius);
			glTexCoord2f(1 - (origin_x + unit_width), origin_y);
			glVertex3f(-chess_top_radius, 1, chess_top_radius);
			glEnd();
		}

		glDisable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glDisable(GL_BLEND);
	glPopMatrix();
}


// Draw one chess's main body.
void chess::draw_chess_body() {
	GLdouble selected_height = 50.0f;
	GLdouble clip_equa_0[4] = {0.0f, -1.0f, 0.0f, chess_half_height};
	GLdouble clip_equa_1[4] = {0.0f, 1.0f, 0.0f, chess_half_height};

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_list[0]);
	// Enable the modulate mode for light rendering.
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glPushMatrix();
		translate_origin();
		translate_point(position[0], position[1]);
		glTranslatef(0, chess_half_height + y_offset * 3, 0);

		// If this chess is selected, it will be lifted.
		if (chessboard->selected == this)
			glTranslatef(0, selected_height, 0);

		// Define the clip equations.
		// The chess model can be seen as a sphere clipped two sides.
		glClipPlane(GL_CLIP_PLANE0, clip_equa_0);
		glClipPlane(GL_CLIP_PLANE1, clip_equa_1);
		glEnable(GL_CLIP_PLANE0);
		glEnable(GL_CLIP_PLANE1);
		glEnable(GL_LIGHTING);

		// Use Quadric object instead of glutSolidSphere to apply texture.
		GLUquadricObj* sphere = NULL;
		sphere = gluNewQuadric();
		gluQuadricTexture(sphere, GL_TRUE);
		gluSphere(sphere, chess_radius, 20, 20);
		gluDeleteQuadric(sphere);

		glDisable(GL_CLIP_PLANE0);
		glDisable(GL_CLIP_PLANE1);
		glDisable(GL_LIGHTING);

		// Draw the top circle to cover the top.
		glPushMatrix();
			glTranslatef(0, chess_half_height, 0);
			draw_circle(chess_top_radius, 180, GL_FILL);
		glPopMatrix();

		glDisable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glPopMatrix();
}


// Draw all the chesses' main body in the chessboard.
void chessboard::draw_chess_body() {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 10; j++) {
			if (chess_map[i][j] == NULL)
				continue;
			chess_map[i][j]->draw_chess_body();
		}
	}
}


// Draw all the chesses' texture (i.e. the picture to identify) in the chessboard.
void chessboard::draw_chess_texture() {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 10; j++) {
			if (chess_map[i][j] == NULL)
				continue;
			chess_map[i][j]->draw_chess_texture();
		}
	}
}


// Draw a simple cross line (similar to +).
void chessboard::draw_single_cross_line() {
	int offset = 5, length = 10;

	int position[4][2] = {
		{-1, -1}, {-1, 1}, {1, -1}, {1, 1}
	};

	glLineWidth(line_width);
	for (int i = 0; i < 4; i++) {
		glBegin(GL_LINE_STRIP);
			glVertex3f(position[i][0] * (offset + length), y_offset, position[i][1] * (offset));
			glVertex3f(position[i][0] * (offset), y_offset, position[i][1] * (offset));
			glVertex3f(position[i][0] * (offset), y_offset, position[i][1] * (offset + length));
		glEnd();
	}
}


// Draw a simple kill line (similar to ¡Á).
void chessboard::draw_single_kill_line() {
	int length = block_size / 4;

	glLineWidth(line_width * 2);
	glPushMatrix();
		glTranslatef(0, chess_height + y_offset * 3, 0);
		glBegin(GL_LINE_STRIP);
			glVertex3f(-length, y_offset, -length);
			glVertex3f(length, y_offset, length);
		glEnd();
		glBegin(GL_LINE_STRIP);
			glVertex3f(-length, y_offset, length);
			glVertex3f(length, y_offset, -length);
		glEnd();
	glPopMatrix();
}


// Draw all the cross lines in the chessboard.
void chessboard::draw_cross_line() {
	int key_point[14][2] = {
		{1, 2}, {7, 2}, {0, 3}, {2, 3}, {4, 3}, {6, 3}, {8, 3},
		{1, 7}, {7, 7}, {0, 6}, {2, 6}, {4, 6}, {6, 6}, {8, 6}
	};

	for (int i = 0; i < 14; i++) {
		glPushMatrix();
			translate_origin();
			translate_point(key_point[i][0], key_point[i][1]);
			glColor3f(0.0, 0.0, 0.0);
			draw_single_cross_line();
		glPopMatrix();
	}
}


// Draw all the block lines in the chessboard.
void chessboard::draw_board_line() {
	int line_map[40][2][2] = {
		{{0, 4}, {0, 5}}, {{8, 4}, {8, 5}},
		{{3, 0}, {5, 2}}, {{5, 0}, {3, 2}},
		{{3, 7}, {5, 9}}, {{5, 7}, {3, 9}}
	};
	int pt = 12;

	// Generate the line array to simplify the code.
	for (int i = 0; i < 9; i++) {
		line_map[pt][0][0] = i;
		line_map[pt][1][0] = i;
		line_map[pt][0][1] = 0;
		line_map[pt][1][1] = 4;
		pt++;
		line_map[pt][0][0] = i;
		line_map[pt][1][0] = i;
		line_map[pt][0][1] = 5;
		line_map[pt][1][1] = 9;
		pt++;
	}
	for (int i = 0; i < 10; i++) {
		line_map[pt][0][0] = 0;
		line_map[pt][1][0] = 8;
		line_map[pt][0][1] = i;
		line_map[pt][1][1] = i;
		pt++;
	}

	glPushMatrix();
		translate_origin();
		glLineWidth(line_width);
		glColor3f(0.0, 0.0, 0.0);
		for (int i = 0; i < pt; i++) {
			glBegin(GL_LINES);
				glVertex3f(-line_map[i][0][0] * block_size, y_offset, -line_map[i][0][1] * block_size);
				glVertex3f(-line_map[i][1][0] * block_size, y_offset, -line_map[i][1][1] * block_size);
			glEnd();
		}
	glPopMatrix();

}


// Draw the pointer in the chessboard.
void chessboard::draw_pointer() {
	int pointer_y_offset = 100;
	int pointer_height = chess_radius * 1.5, pointer_radius = chess_radius / 1.5;

	glPushMatrix();
		translate_origin();
		translate_point(pt_position[0], pt_position[1]);
		glPushMatrix();
			glTranslatef(0, pointer_y_offset, 0);
			glRotatef(-90, 1, 0, 0);
			glColor4f(0, 1, 0, 0.5);

			// Enable the blend mode for tranparency rendering.
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			GLUquadricObj* cone = NULL;
			cone = gluNewQuadric();
			gluQuadricTexture(cone, GL_TRUE);
			gluCylinder(cone, 0, pointer_radius, pointer_height, 20, 20);
			gluDeleteQuadric(cone);

			glDisable(GL_BLEND);

			// Draw a circle the outline the top of the cone to make it more stereoscopic.
			glTranslatef(0, 0, pointer_height);
			glColor3f(0, 0, 0);
			glRotatef(90, 1, 0, 0);

			draw_circle(pointer_radius, 180, GL_LINE);
		glPopMatrix();

		// Draw the red cross line to hint the position of the pointer.
		glPushMatrix();
			glColor3f(1, 0, 0);
			glTranslatef(0, y_offset, 0);

			// If the pointer is above a non-selected chess, lift the cross line.
			if(chess_map[pt_position[0]][pt_position[1]] != NULL)
				if(chess_map[pt_position[0]][pt_position[1]] != selected)
					glTranslatef(0, chess_height + y_offset * 2, 0);
			draw_single_cross_line();
		glPopMatrix();
	glPopMatrix();
}


// Draw the basic chessboard.
void chessboard::draw_board() {
	glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture_list[0]);

		glTranslatef(0, -board_size[1] / 2, 0);
		glScalef(board_size[0], board_size[1], board_size[2]);

		draw_cube(1);

		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}


// Draw the move hint lines when selecting a chess.
void chessboard::draw_move_hint() {
	if (selected == NULL)
		return;
		
	for (int x = 0; x < 9; x++) {
		for (int y = 0; y < 10; y++) {
			glPushMatrix();
				translate_origin();
				translate_point(x, y);

				// If is blocked, skip the hint.
				if (move_map[x][y] == mv_block) {
					glPopMatrix();
					continue;
				}
				// If is a space, draw a green cross line.
				else if (move_map[x][y] == mv_space) {
					glColor3f(0, 1, 0);
					draw_single_cross_line();
				}
				// If is self, draw a yellow cross line.
				else if (move_map[x][y] == mv_self) {
					glColor3f(1, 1, 0);
					draw_single_cross_line();
				}
				// If is an enemy, draw a red kill line.
				else if (move_map[x][y] == mv_enemy) {
					glColor3f(1, 0, 0);
					draw_single_kill_line();
				}
			glPopMatrix();
		}
	}
}


// The main draw function.
void chessboard::draw() {
	// Apply this sequence to avoid overrender.
	draw_chess_body();
	draw_board_line();
	draw_cross_line();
	draw_move_hint();
	draw_pointer();
	draw_board();
	draw_chess_texture();
}


// Initialize the chessboard object into a beginning state.
void chessboard::init_chess() {
	// The initial placement of chesses.
	int init_map[32][4] = {
	{0, 0, pl_black, chariot},
	{1, 0, pl_black, horse},
	{2, 0, pl_black, elephant},
	{3, 0, pl_black, advisor},
	{4, 0, pl_black, general},
	{5, 0, pl_black, advisor},
	{6, 0, pl_black, elephant},
	{7, 0, pl_black, horse},
	{8, 0, pl_black, chariot},
	{1, 2, pl_black, cannon},
	{7, 2, pl_black, cannon},
	{0, 3, pl_black, soldier},
	{2, 3, pl_black, soldier},
	{4, 3, pl_black, soldier},
	{6, 3, pl_black, soldier},
	{8, 3, pl_black, soldier},

	{0, 9, pl_red, chariot},
	{1, 9, pl_red, horse},
	{2, 9, pl_red, elephant},
	{3, 9, pl_red, advisor},
	{4, 9, pl_red, general},
	{5, 9, pl_red, advisor},
	{6, 9, pl_red, elephant},
	{7, 9, pl_red, horse},
	{8, 9, pl_red, chariot},
	{1, 7, pl_red, cannon},
	{7, 7, pl_red, cannon},
	{0, 6, pl_red, soldier},
	{2, 6, pl_red, soldier},
	{4, 6, pl_red, soldier},
	{6, 6, pl_red, soldier},
	{8, 6, pl_red, soldier},
	};

	::memset(chess_map, NULL, sizeof(chess_map));

	for (int i = 0; i < 32; i++) {
		int x = init_map[i][0];
		int y = init_map[i][1];
		chess_map[x][y] = new chess;
		chess_map[x][y]->position[0] = x;
		chess_map[x][y]->position[1] = y;
		chess_map[x][y]->player = (player_type)init_map[i][2];
		chess_map[x][y]->type = init_map[i][3];
		chess_map[x][y]->chessboard = this;
	}

	::memset(move_map, mv_block, sizeof(move_map));
	::memset(pt_position, 0, sizeof(pt_position));
	selected = NULL;
	player = pl_red;

	fltAngle = -90;
}


// Move the pointer by the key and the viewpoint.
void chessboard::move_pointer(char key) {
	// Due to the rotation of the viewpoint,
	// the relative direction keeps changing in different angle.
	// Here gives a coarse code to solve the problem.
	int key_idx;
	int zone_idx = ((int)fltAngle + 135) / 90;
	int move_table[4][4] = {
	{4, 1, 3, 2},
	{2, 4, 1, 3},
	{3, 2, 4, 1},
	{1, 3, 2, 4}
	};

	if (key == 'w')
		key_idx = 0;
	else if (key == 'a')
		key_idx = 1;
	else if (key == 's')
		key_idx = 2;
	else if (key == 'd')
		key_idx = 3;

	int idx = move_table[key_idx][zone_idx];

	if (idx == 1) {
		if (pt_position[0] >= 0 && pt_position[0] < 8) {
			pt_position[0]++;
		}
	}
	if (idx == 2) {
		if (pt_position[0] > 0 && pt_position[0] <= 8) {
			pt_position[0]--;
		}
	}
	if (idx == 3) {
		if (pt_position[1] >= 0 && pt_position[1] < 9) {
			pt_position[1]++;
		}
	}
	if (idx == 4) {
		if (pt_position[1] > 0 && pt_position[1] <= 9) {
			pt_position[1]--;
		}
	}
}


// The main function when <Space> is buttoned (main step function of the game).
void chessboard::select() {
	int x = pt_position[0];
	int y = pt_position[1];
	chess* aim = chess_map[x][y];

	if (selected != NULL) {
		// If one chess is selected, try to go forward.
		// If the position is reachable, go this chess.
		if (move_map[x][y] == mv_enemy || move_map[x][y] == mv_space) {
			// If this step kills any enemy, check if the general is killed.
			// If the general is killed, the player of the killer wins.
			if (move_map[x][y] == mv_enemy)
				if (check_general(x, y)) {
					init_chess();
					return;
				}
					
			// Move the selected chess into the pointer position.
			chess_map[selected->position[0]][selected->position[1]] = NULL;
			chess_map[x][y] = selected;
			selected->position[0] = x;
			selected->position[1] = y;

			// Empty the move map for next moving.
			::memset(move_map, mv_block, sizeof(move_map));
			selected = NULL;
			// Switch the player turn.
			player = player == pl_red ? pl_black : pl_red;
			// SWitch the viewpoint.
			fltAngle = player == pl_red ? -90 : 90;
		}
		// If the position is the same position, this step is seen as cancellation.
		else if (move_map[x][y] == mv_self) {
			::memset(move_map, mv_block, sizeof(move_map));
			selected = NULL;
		}
	}
	else {
		// If no chess is selected, try to select one chess.
		// If selecting a space, do nothing.
		if (aim == NULL)
			return;
		// If selecting a chess of other player, do nothing.
		if (aim->player != player)
			return;
		// If selecting a valid chess, call the chess move function.
		selected = aim;
		selected->move();
	}
}


// Get the player info of a target point.
player_type chessboard::get_chess_player(int x, int y) {
	if (chess_map[x][y] == NULL)
		return pl_null;
	else
		return chess_map[x][y]->player;
}


// Set the move info of a target point.
void chessboard::set_move_map(int x, int y, move_type type) {
	move_map[x][y] = type;
}


// Check whether the general is killed.
bool chessboard::check_general(int aimx, int aimy) {
	if (chess_map[aimx][aimy] == NULL)
		return false;
	return chess_map[aimx][aimy]->type == general;
}


// Draw all the texts.
void draw_text() {
	int height = 200, offset = 50, font_size = 30;
	float zoom_offset = 1.4;

	selectFont(font_size, ANSI_CHARSET, "Comic Sans MS");
	glColor3f(0, 0, 0);

	// Draw the text in four corners to simulate window printing.
	// TODO: Apply screen/window coordinators to print the strings.
	if (fltZoom <= zoom_offset) {
		for (int i = -1; i <= 1; i += 2) {
			glRasterPos3f(i * board_size[0], height - 0 * offset, i * board_size[0]);
			drawString("Press Q/E to Rotate.");
			glRasterPos3f(i * board_size[0], height - 1 * offset, i * board_size[0]);
			drawString("Press W/A/S/D to Move.");
			glRasterPos3f(i * board_size[0], height - 2 * offset, i * board_size[0]);
			drawString("Press Space to Select.");
			glRasterPos3f(i * board_size[0], height - 3 * offset, i * board_size[0]);
			drawString("Press Z/X/C/V to Zoom.");

			string player = "It is turn for ";
			player += my_chessboard.player == pl_red ? "red" : "black";
			player += ".";
			glRasterPos3f(-i * board_size[0], height, i * board_size[0]);
			drawString(player.c_str());
		}
	}

	glColor3f(1, 1, 1);
}


// Initialize the glut environment.
void init_glut_env() {
	// Set the background into a light blue (Good for eyes).
	glClearColor(153.0 / 255.0, 217.0 / 255.0, 234.0 / 255.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glShadeModel(GL_FLAT);

	wood_texture = load_image("wood.bmp");
	chess_texture = load_image("chess.bmp", true);

	// Bind the textures.
	glGenTextures(2, &texture_list[0]);
	glBindTexture(GL_TEXTURE_2D, texture_list[0]);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wood_texture->width, wood_texture->height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, wood_texture->pixeldata);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, texture_list[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, chess_texture->width, chess_texture->height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, chess_texture->pixeldata);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Apply the lighting func (Only shininess and emission).
	GLfloat mat_shininess[] = { 0.0 };
	GLfloat mat_emission[] = { 0.2, 0.2, 0.2, 0.0 };
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
	GLfloat light_position[] = { 1, 1, 1, 0 };		// Directional light for a tilted angle.
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);
}


// Initialize the glut environment in glutDisplayFunc().
void display_glut_env() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fltFOV, 1080.0 / 720.0, 0.1, 5000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Set the viewpoint.
	gluLookAt(fltRange * fltZoom * cos(fltAngle / 180 * 3.14159), fltY0 * fltZoom, fltRange * fltZoom * sin(fltAngle / 180 * 3.14159),
		fltXRef, fltYRef, fltZRef,
		fltXUp, fltYUp, fltZUp);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


// Initialize the whole program.
void initialize(void) {
	init_glut_env();
	my_chessboard.init_chess();
}


// Main display function for glutDisplayFunc()
void display(void) {
	display_glut_env();

	draw_text();
	my_chessboard.draw();

	glFlush();
}


// Keyboard input function for glutKeyboardFunc().
void keyboard_input(unsigned char key, int x, int y)
{	// Camera down.
	if (key == 'c' || key == 'C')
		if (fltY0 > 35)
			fltY0 -= 5;
	// Camera up.
	if (key == 'v' || key == 'V')
		if (fltY0 < 1000)
			fltY0 += 5;
	// Zoom in.
	if (key == 'z' || key == 'Z')
		if (fltZoom > 0.2)
			fltZoom -= 0.1;
	// Zoom out.
	if (key == 'x' || key == 'X')
		if (fltZoom < 4.0)
			fltZoom += 0.1;
	// Rotate anti-clockwise.
	if (key == 'q' || key == 'Q') {
		if (fltAngle >= 180 + 45)
			fltAngle -= 360;
		fltAngle += 1.5;
	}
	// Rotate clockwise.
	if (key == 'e' || key == 'E') {
		if (fltAngle <= -180 + 45)
			fltAngle += 360;
		fltAngle -= 1.5;
	}
	// Pointer go forward.
	if (key == 'w' || key == 'W')
		my_chessboard.move_pointer('w');
	// Pointer go back.
	if (key == 's' || key == 'S')
		my_chessboard.move_pointer('s');
	// Pointer go left.
	if (key == 'a' || key == 'A')
		my_chessboard.move_pointer('a');
	// Pointer go right.
	if (key == 'd' || key == 'D')
		my_chessboard.move_pointer('d');
	// Select in the pointer position.
	if (key == ' ')
		my_chessboard.select();

	glutPostRedisplay();
}


// Main function.
void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1080, 720);
	glutCreateWindow("Chinese Chess - Xiangqi");

	initialize();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard_input);
	glutMainLoop();
}