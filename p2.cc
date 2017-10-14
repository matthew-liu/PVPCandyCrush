extern "C" {
	#include <gtk/gtk.h>
  #include <gdk/gdk.h>
}
#include "utility.h"
#include "manipulate.h"
#include <string>
#include <iostream>
// network & parallel code //
#include "module.h"
#include "./socketCode/ClientSocket.h"
#include <thread>
#include <atomic>
//*************************//
using namespace std;

int maxScore;

GtkWidget *window;
GtkWidget *global_grid;

Gamedef *gamedef;
Gamestate *gamestate;
int coord[2];

Gamedef *p2_gamedef;
Gamestate *p2_gamestate;

static void draw (GtkWidget *window);

static void draw_layout (GtkWidget *layout_grid, Gamestate& state);
static void draw_p2_layout (GtkWidget *layout_grid, Gamestate& state);

static void repaint() {
  gtk_widget_destroy(global_grid);
  draw(window);
}

// network & parallel code //
hw5_net::ClientSocket *clientSocket_ptr;
atomic<int> updated (0);

void thread_read() {
  while(true) {
    string move = Module::readMessage(*clientSocket_ptr);
    if (move == "") {
      cout << "Opponent left the game" << endl;
      break;
    }
    if (move == "{end}") break;
    json_t *root;
    json_error_t error;

    root = json_loads(const_cast<char*>(move.c_str()), 0, &error);

    int row = json_integer_value(json_object_get(root, "row"));
    int column = json_integer_value(json_object_get(root, "column"));
    int direction = json_integer_value(json_object_get(root, "direction"));

    json_decref(root);

    if (direction == 0) Swap((*p2_gamestate).boardcandies->data, row, column, row, column - 1);
    else if (direction == 1) Swap((*p2_gamestate).boardcandies->data, row, column, row, column + 1);
    else if (direction == 2) Swap((*p2_gamestate).boardcandies->data, row, column, row + 1, column);
    else Swap((*p2_gamestate).boardcandies->data, row, column, row - 1, column);
    Manipulate::update(*p2_gamedef, *p2_gamestate);

    (*p2_gamestate).movesmade++;
    updated = 1;
  }
}

void sendMove(int direction) {
    json_t * move = json_object();

    Utility::json_integer_set(move, "row", coord[0]);
    Utility::json_integer_set(move, "column", coord[1]);
    Utility::json_integer_set(move, "direction", direction);

    char* dumped = json_dumps(move, JSON_ENCODE_ANY);
    string s(dumped);
    free(dumped);
    json_decref(move);
    (*clientSocket_ptr).WrappedWrite(s.c_str(), s.length());
}
//*************************//

static void reset_coord () {
  if (coord[0] == -1 && coord[1] == -1) { // default case
    g_print("Can't move: no candy selected\n");
  }
  coord[0] = -1;
  coord[1] = -1;
}

static GtkWidget *image_box( gchar *image_file) {
    GtkWidget *box;
    GtkWidget *image;

    /* Create box for image and label */
    box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_set_border_width (GTK_CONTAINER (box), 2);

    /* Now on to the image stuff */
    image = gtk_image_new_from_file (image_file);


    /* Pack the image and label into the box */
    gtk_box_pack_start (GTK_BOX (box), image, TRUE, TRUE, 3);

    gtk_widget_show (image);

    return box;
}

static void choose (GtkWidget *widget, gpointer data) {
  if (updated) {
    repaint();
    updated = 0;
  }
  long index = (long) data;
  coord[0] = index / (*gamestate).boardcandies->rows;
  coord[1] = index % (*gamestate).boardcandies->rows;
}

static void leftClick (GtkWidget *widget, gpointer data) {
  if (Swap((*gamestate).boardcandies->data, coord[0], coord[1], coord[0], coord[1] - 1) &&
  	Manipulate::update(*gamedef, *gamestate)) {       
    (*gamestate).movesmade++;
    sendMove(0);
    repaint();	
  } else {
  	Swap((*gamestate).boardcandies->data, coord[0], coord[1], coord[0], coord[1] - 1);
  }
  reset_coord();
}

static void rightClick (GtkWidget *widget, gpointer data) {
  if (Swap((*gamestate).boardcandies->data, coord[0], coord[1], coord[0], coord[1] + 1) &&
  	Manipulate::update(*gamedef, *gamestate)) {
    (*gamestate).movesmade++;
    sendMove(1);
    repaint();	
  } else {
  	Swap((*gamestate).boardcandies->data, coord[0], coord[1], coord[0], coord[1] + 1);
  }
  reset_coord();
}

static void upClick (GtkWidget *widget, gpointer data) {
  if (Swap((*gamestate).boardcandies->data, coord[0], coord[1], coord[0] + 1, coord[1]) &&
  	Manipulate::update(*gamedef, *gamestate)) {
    (*gamestate).movesmade++;
    sendMove(2);
    repaint();	
  } else {
  	Swap((*gamestate).boardcandies->data, coord[0], coord[1], coord[0] + 1, coord[1]);
  }
  reset_coord();
}

static void downClick (GtkWidget *widget, gpointer data) {
  if (Swap((*gamestate).boardcandies->data, coord[0], coord[1], coord[0] - 1, coord[1]) &&
  	Manipulate::update(*gamedef, *gamestate)) {
    (*gamestate).movesmade++;
    sendMove(3);
    repaint();	
  } else {
  	Swap((*gamestate).boardcandies->data, coord[0], coord[1], coord[0] - 1, coord[1]);
  }
  reset_coord();
}

static GtkWidget* draw_p1 () {
  GtkWidget *grid;
  GtkWidget *layout_grid;
  GtkWidget *button_grid;
  GtkWidget *label;
  GtkWidget *p2_label;

  GtkWidget *button_l;
  GtkWidget *button_r;
  GtkWidget *button_u;
  GtkWidget *button_d;
  GtkWidget *button_l_layout;
  GtkWidget *button_r_layout;
  GtkWidget *button_u_layout;
  GtkWidget *button_d_layout;

  /* Here we construct the container that is going pack our buttons */
  grid = gtk_grid_new ();
  button_grid = gtk_grid_new ();
  layout_grid = gtk_grid_new ();

  gtk_grid_attach (GTK_GRID (grid), layout_grid, 0, 0, 1, 1);
  gtk_grid_attach (GTK_GRID (grid), button_grid, 1, 0, 1, 1);

  char* path;

  button_l = gtk_button_new ();
  g_signal_connect (button_l, "clicked", G_CALLBACK (leftClick), NULL);
  path = "images/direction/left.png";
  button_l_layout = image_box (path);
  gtk_container_add (GTK_CONTAINER (button_l), button_l_layout);

  button_r = gtk_button_new ();
  g_signal_connect (button_r, "clicked", G_CALLBACK (rightClick), grid);
  path = "images/direction/right.png";
  button_r_layout = image_box (path);
  gtk_container_add (GTK_CONTAINER (button_r), button_r_layout);

  button_u = gtk_button_new ();
  g_signal_connect (button_u, "clicked", G_CALLBACK (upClick), grid);
  path = "images/direction/up.png";
  button_u_layout = image_box (path);
  gtk_container_add (GTK_CONTAINER (button_u), button_u_layout);

  button_d = gtk_button_new ();
  g_signal_connect (button_d, "clicked", G_CALLBACK (downClick), grid);
  path = "images/direction/down.png";
  button_d_layout = image_box (path);
  gtk_container_add (GTK_CONTAINER (button_d), button_d_layout);

  char str[45];
  sprintf(str, "Your moves made: %d\n   Your Score: %d\n", (*gamestate).movesmade, (*gamestate).currentscore);
  label = gtk_label_new(str);

  gtk_grid_attach (GTK_GRID (button_grid), label,    0, 0, 1, 1);
  gtk_grid_attach (GTK_GRID (button_grid), button_l, 0, 1, 1, 1);
  gtk_grid_attach (GTK_GRID (button_grid), button_r, 0, 2, 1, 1);
  gtk_grid_attach (GTK_GRID (button_grid), button_u, 0, 3, 1, 1);
  gtk_grid_attach (GTK_GRID (button_grid), button_d, 0, 4, 1, 1);

  char p2_str[45];
  sprintf(p2_str, "\nOpponent's moves made: %d\n   Opponent's Score: %d", (*p2_gamestate).movesmade, (*p2_gamestate).currentscore);
  p2_label = gtk_label_new(p2_str);

  gtk_grid_attach (GTK_GRID (button_grid), p2_label, 0, 5, 1, 1);

  gtk_grid_set_row_spacing(GTK_GRID (button_grid), 4);
  gtk_grid_set_column_spacing(GTK_GRID (grid), 10);
  draw_layout(layout_grid, *gamestate);

  return grid;
}

static GtkWidget* draw_p2 () {
  GtkWidget *layout_grid;

  layout_grid = gtk_grid_new ();

  draw_p2_layout(layout_grid, *p2_gamestate);

  return layout_grid;
}

static void draw (GtkWidget *window) {

  if ((*gamestate).currentscore >= maxScore || (*p2_gamestate).currentscore >= maxScore) {
    (*clientSocket_ptr).WrappedWrite("{end}", 5);
    char str[120];
    if ((*gamestate).currentscore >= maxScore) {
      sprintf(str, "Congradulations!!! You Win :)\n\n Your final moves made: [%d]\n Your final Score: [%d]\n\n Opponent's final moves made: [%d]\n Opponent's final Score: [%d]\n", (*gamestate).movesmade, 
      (*gamestate).currentscore, (*p2_gamestate).movesmade, (*p2_gamestate).currentscore);
    } else {
      sprintf(str, "Ooops... You Lose :(\n\n Your final moves made: [%d]\n Your final Score: [%d]\n\n Opponent's final moves made: [%d]\n Opponent's final Score: [%d]\n", (*gamestate).movesmade, 
      (*gamestate).currentscore, (*p2_gamestate).movesmade, (*p2_gamestate).currentscore);
    }
    GtkWidget *label = gtk_label_new(str);
    gtk_container_add (GTK_CONTAINER (window), label);

  } else {
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *p1_grid = draw_p1();
    GtkWidget *p2_grid = draw_p2();

    gtk_grid_attach (GTK_GRID (grid), p1_grid, 0, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (grid), p2_grid, 1, 0, 1, 1);

    gtk_grid_set_column_spacing(GTK_GRID (grid), 10);

    global_grid = grid;

    /* Pack the container in the window */
    gtk_container_add (GTK_CONTAINER (window), grid);
  }
  gtk_widget_show_all (window);
}

static void draw_p2_layout (GtkWidget *layout_grid, Gamestate& state) {
  GtkWidget *unit;
  GtkWidget *unit_pic;
  int* payload;
  int num;
  int rows = state.boardcandies->rows;
  int columns = state.boardcandies->columns;

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (Get(state.boardcandies->data, i, j, (Payload*)&payload)) {
        num = payload[0];

        unit = gtk_button_new ();

        string filePath = "images/40x40/";

        if (num == 0) filePath.append("blue.png");
        if (num == 1) filePath.append("green.png");
        if (num == 2) filePath.append("orange.png");
        if (num == 3) filePath.append("purple.png");
        if (num == 4) filePath.append("red.png");
        if (num == 5) filePath.append("yellow.png");

        unit_pic = image_box (const_cast<char*>(filePath.c_str()));
        gtk_container_add (GTK_CONTAINER (unit), unit_pic);
        gtk_grid_attach (GTK_GRID (layout_grid), unit, j, (rows - i), 1, 1);
      }
    }
  }

  gtk_grid_set_row_spacing(GTK_GRID (layout_grid), 2);
  gtk_grid_set_column_spacing(GTK_GRID (layout_grid), 2);
}

static void draw_layout (GtkWidget *layout_grid, Gamestate& state) {
  GtkWidget *unit;
  GtkWidget *unit_pic;
  int* payload;
  int num;
  int rows = state.boardcandies->rows;
  int columns = state.boardcandies->columns;

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (Get(state.boardcandies->data, i, j, (Payload*)&payload)) {
      	num = payload[0];

        unit = gtk_button_new ();
        long index = i * rows + j;
        g_signal_connect (unit, "clicked", G_CALLBACK (choose), (void*) index);

        string filePath = "images/40x40/";

        if (num == 0) filePath.append("blue.png");
        if (num == 1) filePath.append("green.png");
        if (num == 2) filePath.append("orange.png");
        if (num == 3) filePath.append("purple.png");
        if (num == 4) filePath.append("red.png");
        if (num == 5) filePath.append("yellow.png");

        unit_pic = image_box (const_cast<char*>(filePath.c_str()));
        gtk_container_add (GTK_CONTAINER (unit), unit_pic);
        gtk_grid_attach (GTK_GRID (layout_grid), unit, j, (rows - i), 1, 1);
      }
    }
  }

  gtk_grid_set_row_spacing(GTK_GRID (layout_grid), 2);
  gtk_grid_set_column_spacing(GTK_GRID (layout_grid), 2);
}

static void activate (GtkApplication *app, gpointer user_data) {

  /* create a new window, and set its title */
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "CandyCrush!");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

  draw(window);

}

static void setUp() {
  // read maxScore from p1
  string score = Module::readMessage(*clientSocket_ptr);

  json_t *root;
  json_error_t error;

  root = json_loads(const_cast<char*>(score.c_str()), 0, &error);
  maxScore = json_integer_value(json_object_get(root, "maxScore"));

  json_decref(root);

  // read json_file from p1
  string buf_str = Module::readMessage(*clientSocket_ptr);

  FILE* input = fopen("temp", "w+");
  fprintf(input, "%s", buf_str.c_str());
  fclose(input);

  Utility::constructFromFile_Client("temp", *gamedef, *gamestate);
  Manipulate::update(*gamedef, *gamestate);

  Utility::constructFromFile_Client("temp", *p2_gamedef, *p2_gamestate);
  Manipulate::update(*p2_gamedef, *p2_gamestate);

  remove("temp");
}

int main (int argc, char **argv) {

  if (argc != 2 && argc != 3) {
      cout << "Usage: " << argv[0] << " [serverName]" << endl;
      cout << "  json file & maxScore are sent from p1" << endl;
      exit(1);
  }

  Gamedef n_gamedef;
  Gamestate n_gamestate;

  gamedef = &n_gamedef;
  gamestate = &n_gamestate;

  Gamedef n_p2_gamedef;
  Gamestate n_p2_gamestate;

  p2_gamedef = &n_p2_gamedef;
  p2_gamestate = &n_p2_gamestate;

  GtkApplication *app;

  coord[0] = -1;
  coord[1] = -1;

  int status = 0;

  try {
    string serverName(argv[1]);
    hw5_net::ClientSocket clientSocket(serverName, 33360);
    clientSocket_ptr = &clientSocket;

    setUp();

    // start receive thread
    thread t_read = thread(thread_read);
    // start the app
    app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), 0, NULL);
    g_object_unref (app);

    Utility::outputToFile(*gamedef, *gamestate);

    t_read.join();

  } catch (string errString) {
    cerr << errString << endl;
    return EXIT_FAILURE;
  }

  return status;
}
