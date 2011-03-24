#include <gtk/gtk.h>


gboolean
on_dlgAbout_delete_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_label_about_realize                 (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_button_aboutclosed_clicked          (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_dlgConfig_delete_event              (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_dlgConfig_realize                   (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_button_add_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_clist_searchpaths_select_row        (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_button_up_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_dglConfig_button_down_clicked       (GtkButton       *button,
                                        gpointer         user_data);

void
on_dglConfig_button_delete_clicked     (GtkButton       *button,
                                        gpointer         user_data);

void
on_dlgConfig_button_help_clicked       (GtkButton       *button,
                                        gpointer         user_data);

void
on_dlgConfig_button_ok_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_dlgConfig_button_cancel_clicked     (GtkButton       *button,
                                        gpointer         user_data);

void
on_dlgConfig_clist_skins_select_row    (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);


void
on_dlgConfig_check_aspectratio_toggled (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_button_e_add_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_clist_extensions_select_row         (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_button_e_up_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_e_down_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_e_delete_clicked             (GtkButton       *button,
                                        gpointer         user_data);
