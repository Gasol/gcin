#include "gcin.h"
#include "intcode.h"
#include "pho.h"
#include "gst.h"
#include "im-client/gcin-im-client-attr.h"
#include "win1.h"
#include "gcin-module.h"
#include "gcin-module-cb.h"

GtkWidget *gwin_int;
extern GCIN_module_main_functions gmf;
extern int current_intcode;
void module_show_win();

static GtkWidget *button_int;
static GtkWidget *labels_int[MAX_INTCODE];

static struct {
  char *name;
} int_sel[]={
  {"Big5"},
  {"UTF-32(U+)"},
};
static int int_selN = sizeof(int_sel)/sizeof(int_sel[0]);

static GtkWidget *opt_int_opts;

static void minimize_win()
{
  gtk_window_resize(GTK_WINDOW(gwin_int), 1, 1);
}

static void adj_intcode_buttons()
{
  int i;

  if (current_intcode==INTCODE_UTF32) {
    for(i=4;i < MAX_INTCODE; i++)
      gtk_widget_show(labels_int[i]);
  } else {
    for(i=4;i < MAX_INTCODE; i++)
      gtk_widget_hide(labels_int[i]);
  }

  minimize_win();
}


static void cb_select( GtkWidget *widget, gpointer data)
{
  current_intcode = gtk_combo_box_get_active (GTK_COMBO_BOX (widget));
  adj_intcode_buttons();
}

static GtkWidget *create_int_opts()
{

  GtkWidget *hbox = gtk_hbox_new (FALSE, 1);

  opt_int_opts = gtk_combo_box_new_text ();
#if !GTK_CHECK_VERSION(2,4,0)
  GtkWidget *menu_int_opts = gtk_menu_new ();
#endif
  gtk_box_pack_start (GTK_BOX (hbox), opt_int_opts, FALSE, FALSE, 0);

  int i;
  for(i=0; i < int_selN; i++) {
#if GTK_CHECK_VERSION(2,4,0)
    gtk_combo_box_append_text (GTK_COMBO_BOX_TEXT (opt_int_opts), int_sel[i].name);
#else
    GtkWidget *item = gtk_menu_item_new_with_label (int_sel[i].name);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu_int_opts), item);
#endif
  }

#if !GTK_CHECK_VERSION(2,4,0)
  gtk_option_menu_set_menu (GTK_OPTION_MENU (opt_int_opts), menu_int_opts);
#endif
  gtk_combo_box_set_active (GTK_COMBO_BOX (opt_int_opts), current_intcode);
  g_signal_connect (G_OBJECT (opt_int_opts), "changed", G_CALLBACK (cb_select), NULL);

  return hbox;
}


void disp_int(int index, char *intcode)
{
  gtk_label_set_text(GTK_LABEL(labels_int[index]), intcode);
}

static char full_space[]="　";

void clear_int_code(int index)
{
  gtk_label_set_text(GTK_LABEL(labels_int[index]), full_space);
}


void clear_int_code_all()
{
  int i;

  for(i=0; i < MAX_INTCODE; i++)
    clear_int_code(i);
}

void module_hide_win()
{
  if (!gwin_int)
    return;
  gtk_widget_hide(gwin_int);
  dbg("hide_win_int\n");
}

extern gboolean force_show;
extern int intcode_cin;

void module_move_win(int x, int y)
{
  if (!gwin_int)
    return;

//  dbg("move_win_int %d %d\n",x,y);

  gtk_window_get_size(GTK_WINDOW(gwin_int), gmf.mf_win_xl, gmf.mf_win_yl);

  if (x + *gmf.mf_win_xl > *gmf.mf_dpy_xl)
    x = *gmf.mf_dpy_xl - *gmf.mf_win_xl;
  if (x < 0)
    x = 0;

  if (y + *gmf.mf_win_yl > *gmf.mf_dpy_yl)
    y = *gmf.mf_dpy_yl - *gmf.mf_win_yl;
  if (y < 0)
    y = 0;

  *gmf.mf_win_x = x;  *gmf.mf_win_y = y;
  gtk_window_move(GTK_WINDOW(gwin_int), x, y);
}


void create_win_intcode()
{
  if (gwin_int) {
    module_show_win();
    return;
  }

  gwin_int = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_has_resize_grip(GTK_WINDOW(gwin_int), FALSE);
  gtk_window_set_resizable(GTK_WINDOW(gwin_int), FALSE);

#if WIN32
  gmf.mf_set_no_focus(gwin_int);
#endif

//  gtk_window_set_default_size(GTK_WINDOW (gwin_int), 1, 1);
  gtk_container_set_border_width (GTK_CONTAINER (gwin_int), 0);

  GdkWindow *gdkwin = gtk_widget_get_window(gwin_int);

  GtkWidget *frame = gtk_frame_new(NULL);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 0);
  gtk_container_add (GTK_CONTAINER(gwin_int), frame);


  GtkWidget *hbox_top = gtk_hbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (frame), hbox_top);

  GtkWidget *button_intcode = gtk_button_new_with_label(_(_L("內碼")));
  g_signal_connect (G_OBJECT (button_intcode), "button_press_event",
        G_CALLBACK (gmf.mf_inmd_switch_popup_handler), NULL);
  gtk_box_pack_start (GTK_BOX (hbox_top), button_intcode, FALSE, FALSE, 0);

  button_int = gtk_button_new();
  gtk_container_set_border_width (GTK_CONTAINER (button_int), 0);
  gtk_box_pack_start (GTK_BOX (hbox_top), button_int, FALSE, FALSE, 0);
  GtkWidget *hbox_int = gtk_hbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (button_int), hbox_int);

  int i;
  for(i=0; i < MAX_INTCODE;i ++) {
    GtkWidget *label = gtk_label_new(full_space);
    labels_int[i] = label;
    gtk_box_pack_start (GTK_BOX (hbox_int), label, FALSE, FALSE, 0);
    gmf.mf_set_label_font_size(label, *gmf.mf_gcin_font_size);
  }

  GtkWidget *intsel = create_int_opts();
  gtk_box_pack_start (GTK_BOX (hbox_top), intsel, FALSE, FALSE, 0);

  gtk_widget_show_all (gwin_int);

  gtk_widget_realize (gwin_int);
#if WIN32
  gmf.mf_win32_init_win(gwin_int);
#else
  set_no_focus(gwin_int);
#endif

  adj_intcode_buttons();
  minimize_win();

//  dbg("create %x\n",gwin_int);
}

void module_show_win()
{
  if (!gwin_int) {
    create_win_intcode();
//    return;
  }

#if 0
  if (gcin_pop_up_win && !intcode_cin && !force_show)
    return;
#endif

#if UNIX
  if (!GTK_WIDGET_VISIBLE(gwin_int))
#endif
  {
    gtk_widget_show(gwin_int);
  }

//  dbg("show_win_int %x\n", gwin_int);
  gtk_widget_show(gwin_int);
  module_move_win(*gmf.mf_win_x, *gmf.mf_win_y);
  gtk_window_present(GTK_WINDOW(gwin_int));
}


void module_win_geom()
{
  if (!gwin_int)
    return;
  gtk_window_get_position(GTK_WINDOW(gwin_int), gmf.mf_win_x, gmf.mf_win_y);

  gmf.mf_get_win_size(gwin_int, gmf.mf_win_xl, gmf.mf_win_yl);
}


int module_win_visible()
{
  return GTK_WIDGET_VISIBLE(gwin_int);
}


void module_change_font_size()
{
}
