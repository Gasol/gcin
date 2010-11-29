#include "gcin.h"
#include "pho.h"

#if USE_TSIN
void flush_tsin_buffer();
#endif

PIN_JUYIN *pin_juyin;

int text_pho_N=3;

gboolean b_use_full_space = TRUE;

static char text_pho[6][CH_SZ];

void bell()
{
  if (gcin_bell_off)
    return;

#if UNIX
  XBell(dpy, gcin_bell_volume);
#else
  gdk_beep();
#endif
//  abort();
}

void case_inverse(KeySym *xkey, int shift_m)
{
  if (*xkey > 0x7e)
    return;

  if (shift_m) {
    if (islower(*xkey))
      *xkey-=0x20;
  } else
  if (isupper(*xkey))
    *xkey+=0x20;
}

gint64 current_time()
{
#if WIN32
  gint64 v = (gint64)GetTickCount()*1000;
//  dbg("v %lld\n", v);
  return v;
#else
  struct timeval tval;

  gettimeofday(&tval, NULL);
  return (gint64)tval.tv_sec * 1000000 + tval.tv_usec;
#endif
}

void disp_pho_sub(GtkWidget *label, int index, char *pho)
{
  if (!label)
    return;

  if (index>=text_pho_N)
    return;


  if (pho[0]==' ' && !pin_juyin) {
    u8cpy(text_pho[index], _(_L("　")));
  }
  else {
    u8cpy(text_pho[index], pho);
  }
#if UNIX
  char s[text_pho_N * CH_SZ+1];
#else
  char *s = new char[text_pho_N * CH_SZ+1];
#endif

  int tn = 0;
  int i;
  for(i=0; i < text_pho_N; i++) {
    int n = utf8cpy(s + tn, text_pho[i]);
    tn += n;
  }

//  gtk_widget_show(label);
  gtk_label_set_text(GTK_LABEL(label), s);
#if WIN32
  delete s;
#endif
}

void exec_gcin_setup()
{
#if DEBUG
  dbg("exec gcin\n");
#endif
#if UNIX
  if (geteuid() < 100 || getegid() < 100)
    return;
#else
  if (getenv("WIN_LOGON"))
    return;
#endif

  char pidstr[32];
  sprintf(pidstr, "GCIN_PID=%d",
#if UNIX
	  getpid()
#else
	  GetCurrentProcessId()
#endif
  );
  putenv(pidstr);
#if UNIX
  system(GCIN_BIN_DIR"/gcin-setup &");
#else
  win32exec("gcin-setup.exe");
#endif
}

void set_label_font_size(GtkWidget *label, int size)
{
  if (!label)
    return;

  PangoContext *pango_context = gtk_widget_get_pango_context (label);
  PangoFontDescription* font=pango_context_get_font_description
       (pango_context);
  pango_font_description_set_family(font, gcin_font_name);
  pango_font_description_set_size(font, PANGO_SCALE * size);
  gtk_widget_modify_font(label, font);
}

// the width of ascii space in firefly song
void set_label_space(GtkWidget *label)
{
  gtk_label_set_text(GTK_LABEL(label), "\xe3\x80\x80");
  return;
}

void set_no_focus(GtkWidget *win)
{
#if UNIX
  gdk_window_set_override_redirect(gtk_widget_get_window(win), TRUE);
#else
  gtk_window_set_decorated(GTK_WINDOW(win), FALSE);
  gtk_window_set_keep_above(GTK_WINDOW(win), TRUE);
  gtk_window_set_accept_focus(GTK_WINDOW(win), FALSE);
  gtk_window_set_type_hint(GTK_WINDOW(win), GDK_WINDOW_TYPE_HINT_TOOLTIP);
  gtk_window_set_skip_taskbar_hint(GTK_WINDOW(win), TRUE);
#endif

  gtk_window_set_accept_focus(GTK_WINDOW(win), FALSE);
  gtk_window_set_focus_on_map (GTK_WINDOW(win), FALSE);
}

#if !USE_TSIN
void add_to_tsin_buf(){}
void add_to_tsin_buf_str(){}
void build_ts_gtab(){}
void change_tsin_color(){}
void change_tsin_font_size(){}
void change_win0_style(){}
void clear_ch_buf_sel_area(){}
void clear_tsin_buffer(){}
void destroy_win0(){}
void destroy_win1(){}
void free_tsin(){}
void load_ts_gtab(){}
void load_tsin_db(){}
void putbuf(){}
void tsin_remove_last(){}
void tsin_reset_in_pho(){}
void tsin_set_eng_ch(){}
void tsin_toggle_half_full(){}
#endif
