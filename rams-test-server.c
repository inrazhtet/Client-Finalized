/**
 * rams-test-server.c
 *   A very simple DBus server used to help the RAMS test their code.
 */


// +---------------------+--------------------------------------------
// | Notes and Questions |
// +---------------------+

/*
  We create a lot of new GVariants.  Should we also explicitly free them?
  Since this is a test program, I'm not going to worry about it.  However,
  for a production program, we need to think more about the issue and
  check the documentation for g_dbus_method_invocation_return_value and
  g_dbus_new.

 */


// +---------+--------------------------------------------------------
// | Headers |
// +---------+

#include <gio/gio.h>		// For all the DBus communcation
#include <stdlib.h>		// For rand, among other things
#include <stdio.h>		// For printf and the ilk
#include <string.h>		// For strcmp


// +--------------------+---------------------------------------------
// | Introspection Data |
// +--------------------+

static GDBusNodeInfo *introspection_data = NULL;

/* Introspection data for the service we are exporting */
static const gchar introspection_xml[] =
  "<node>"
  "  <interface name='edu.grinnell.cs.glimmer.RAMStest'>"
  "    <method name='v2v'>"
  "      <annotation name='org.gtk.GDBus.Annotation'"
  "                  value='void -> void'/>"
  "      <annotation name='org.gtk.GDBus.Annotation'"
  "                  value='Prints Hello'/>"
  "    </method>"
  "    <method name='i2v'>"
  "      <annotation name='org.gtk.GDBus.Annotation'"
  "                  value='int -> void'/>"
  "      <annotation name='org.gtk.GDBus.Annotation'"
  "                  value='Prints the parameter'/>"
  "      <arg type='i' name='x' direction='in'/>"
  "    </method>"
  "    <method name='v2i'>"
  "      <annotation name='org.gtk.GDBus.Annotation'"
  "                  value='void -> int'/>"
  "      <annotation name='org.gtk.GDBus.Annotation'"
  "                  value='Returns the answer'/>"
  "      <arg type='i' name='answer' direction='out'/>"
  "    </method>"
  "    <method name='i2i'>"
  "      <annotation name='org.gtk.GDBus.Annotation'"
  "                  value='int -> int'/>"
  "      <annotation name='org.gtk.GDBus.Annotation'"
  "                  value='Squares the parameter'/>"
  "      <arg type='i' name='x' direction='in'/>"
  "      <arg type='i' name='result' direction='out'/>"
  "    </method>"
  "    <method name='v2ii'>"
  "      <annotation name='org.gtk.GDBus.Annotation'"
  "                  value='void -> int x int'/>"
  "      <annotation name='org.gtk.GDBus.Annotation'"
  "                  value='Two random numbers'/>"
  "      <arg type='i' name='x' direction='out'/>"
  "      <arg type='i' name='y' direction='out'/>"
  "    </method>"
  "    <method name='ii2v'>"
  "      <annotation name='org.gtk.GDBus.Annotation'"
  "                  value='int x int -> void'/>"
  "      <annotation name='org.gtk.GDBus.Annotation'"
  "                  value='Prints both parameters'/>"
  "      <arg type='i' name='x' direction='in'/>"
  "      <arg type='i' name='y' direction='in'/>"
  "    </method>"
  "    <method name='ii2i'>"
  "      <annotation name='org.gtk.GDBus.Annotation'"
  "                  value='int x int -> int'/>"
  "      <annotation name='org.gtk.GDBus.Annotation'"
  "                  value='Some funky function of two params'/>"
  "      <arg type='i' name='x' direction='in'/>"
  "      <arg type='i' name='y' direction='in'/>"
  "      <arg type='i' name='result' direction='out'/>"
  "    </method>"
  "    <method name='s2v'>"
  "      <annotation name='org.gtk.GDBus.Annotation'"
  "                  value='string -> void'/>"
  "      <annotation name='org.gtk.GDBus.Annotation'"
  "                  value='Prints the parameter'/>"
  "      <arg type='s' name='x' direction='in'/>"
  "    </method>"
  "    <method name='v2s'>"
  "      <annotation name='org.gtk.GDBus.Annotation'"
  "                  value='void -> string'/>"
  "      <annotation name='org.gtk.GDBus.Annotation'"
  "                  value='Returns the answer'/>"
  "      <arg type='s' name='answer' direction='out'/>"
  "    </method>"
  "    <method name='i2s'>"
  "      <annotation name='org.gtk.GDBus.Annotation'"
  "                  value='int -> string'/>"
  "      <annotation name='org.gtk.GDBus.Annotation'"
  "                  value='Converts int to string'/>"
  "      <arg type='i' name='x' direction='in'/>"
  "      <arg type='s' name='answer' direction='out'/>"
  "    </method>"
  "  </interface>"
  "</node>";


// +-------------------+----------------------------------------------
// | Standard Handlers |
// +-------------------+

static void
handle_method_call (GDBusConnection       *connection,
                    const gchar           *sender,
                    const gchar           *object_path,
                    const gchar           *interface_name,
                    const gchar           *method_name,
                    GVariant              *parameters,
                    GDBusMethodInvocation *invocation,
                    gpointer               user_data)
{
  // Note: Handlers are in alphabetical order, rather than order of
  //   increasing complexity

  // This stupid test is mostly a way for me to put everything else in
  // an "else if" clause.
  if (0)
    ;

  // Handle i2i
  else if (g_strcmp0 (method_name, "i2i") == 0)
    {
      int x;
      g_variant_get (parameters, "(i)", &x);
#ifdef VERBOSE
      printf ("i2i(%d)\n", x);
#endif
      g_dbus_method_invocation_return_value (invocation, 
                                             g_variant_new ("(i)", x*x));
    } // i2i

  // Handle i2s
  else if (g_strcmp0 (method_name, "i2s") == 0)
    {
      int x;		// Our input value
      gchar *response;	// Our output value
      g_variant_get (parameters, "(i)", &x);
#ifdef VERBOSE
      printf ("i2s(%d)\n", x);
#endif
      // g_strdup_printf is an awesome function that builds a new string
      // using the structure of printf.  You will need to free that string
      // at some point, using g_free.
      response = g_strdup_printf ("%d", x);
      g_dbus_method_invocation_return_value (invocation, 
                                             g_variant_new ("(s)", response));
      // Ooh!  We can do things after we return a value.  The wonder of
      // asynchronous code.
      g_free (response);
    } // i2s

  // Handle i2v
  else if (g_strcmp0 (method_name, "i2v") == 0)
    {
      int x;
      g_variant_get (parameters, "(i)", &x);
#ifdef VERBOSE
      printf ("i2v(%d)\n", x);
#endif
      printf ("%d\n", x);
      g_dbus_method_invocation_return_value (invocation, NULL);
    } // i2v

  // Handle ii2i
  else if (g_strcmp0 (method_name, "ii2i") == 0)
    {
      int x, y;
      g_variant_get (parameters, "(ii)", &x, &y);
      int result = 10*x + y;
#ifdef VERBOSE
      printf ("ii2v(%d,%d)\n", x,y);
#endif
      printf ("(%d,%d)\n", x, y);
      g_dbus_method_invocation_return_value (invocation, 
                                             g_variant_new ("(i)", result));
    } // ii2i

  // Handle ii2v
  else if (g_strcmp0 (method_name, "ii2v") == 0)
    {
      int x, y;
      g_variant_get (parameters, "(ii)", &x, &y);
#ifdef VERBOSE
      printf ("ii2v(%d,%d)\n", x,y);
#endif
      printf ("(%d,%d)\n", x, y);
      g_dbus_method_invocation_return_value (invocation, NULL);
    } // ii2v

  // Handle s2i
  else if (g_strcmp0 (method_name, "s2i") == 0)
    {
      const gchar *str;	// The parameter
      int len;		// The result
      g_variant_get (parameters, "(&s)", &str);
#ifdef VERBOSE
      printf ("s2i(%s)\n", str);
#endif
      len = strlen (str);
      g_dbus_method_invocation_return_value (invocation, 
                                             g_variant_new ("(i)", len));
    } // s2i

  // Handle s2v
  else if (g_strcmp0 (method_name, "s2v") == 0)
    {
      const gchar *x;
      g_variant_get (parameters, "(&s)", &x);
#ifdef VERBOSE
      printf ("s2v(%s)\n", x);
#endif
      printf ("%s\n", x);
      g_dbus_method_invocation_return_value (invocation, NULL);
    } // s2v

  // Handle v2i
  else if (g_strcmp0 (method_name, "v2i") == 0)
    {
#ifdef VERBOSE
      printf ("v2i()\n");
#endif
      g_dbus_method_invocation_return_value (invocation,
                                             g_variant_new ("(i)", 42));
    } // v2i

  // Handle v2ii
  else if (g_strcmp0 (method_name, "v2ii") == 0)
    {
#ifdef VERBOSE
      printf ("v2ii()\n");
#endif
      g_dbus_method_invocation_return_value (invocation,
                                             g_variant_new ("(ii)", 
					                    rand (),
							    rand ()));
    } // v2ii

  // Handle v2s
  else if (g_strcmp0 (method_name, "v2s") == 0)
    {
#ifdef VERBOSE
      printf ("v2s()\n");
#endif
      g_dbus_method_invocation_return_value (invocation,
                                            g_variant_new ("(s)", "Two"));
    } // v2i

  // Handle v2v.
  else if (g_strcmp0 (method_name, "v2v") == 0)
    {
#ifdef VERBOSE
      printf ("v2v()\n")
#endif
      printf ("Hello world.\n");
      // Note: We use NULL as the second actual parameter of the
      // return_value function to say "return nothing, other than
      // a note that you've returned."
      g_dbus_method_invocation_return_value (invocation, NULL);
    } // v2v

  // Handle all other method calls
  else
    {
#ifdef VERBOSE
      printf ("handle_method (%s ...): Unknown method\n" method_name);
#endif
      gchar *response = g_strdup_printf ("Unknown method: %s", method_name);
      g_dbus_method_invocation_return_error (invocation, 
                                             G_IO_ERROR,
					     G_IO_ERROR_NOT_FOUND,
					     response);
      g_free (response);
    } // if the name is not known

} // handle_method_call
 
static GVariant *
handle_get_property (GDBusConnection  *connection,
                     const gchar      *sender,
                     const gchar      *object_path,
                     const gchar      *interface_name,
                     const gchar      *property_name,
                     GError          **error,
                     gpointer          user_data)
{
  g_set_error (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
               "No property %s found in interface %s.",
	       property_name, 
	       interface_name);
  // NULL signals failure.
  return NULL;
} // handle_set_property

static gboolean
handle_set_property (GDBusConnection  *connection,
                     const gchar      *sender,
                     const gchar      *object_path,
                     const gchar      *interface_name,
                     const gchar      *property_name,
                     GVariant         *value,
                     GError          **error,
                     gpointer          user_data)
{
  g_set_error (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
               "No property %s found in interface %s.",
	       property_name, 
	       interface_name);
  // false signals failure.
  return FALSE;
} // handle_get_property

/* Our three handlers */
static const GDBusInterfaceVTable interface_vtable =
{
  handle_method_call,
  handle_get_property,
  handle_set_property
};


// +----------------------------+-------------------------------------
// | Standard DBus Server Setup |
// +----------------------------+

static void
on_bus_acquired (GDBusConnection *connection,
                 const gchar     *name,
                 gpointer         user_data)
{
  guint registration_id;

#ifdef VERBOSE
  fprintf (stderr, "Yay, I got name %s on the bus.\n", name);
#endif

  registration_id = g_dbus_connection_register_object (connection,
                                                       "/edu/grinnell/cs/glimmer/RAMStest",
                                                       introspection_data->interfaces[0],
                                                       &interface_vtable,
                                                       NULL,  /* user_data */
                                                       NULL,  /* user_data_free_func */
                                                       NULL); /* GERROR */
} // on_bus_acquired

static void
on_name_acquired (GDBusConnection *connection,
                  const gchar     *name,
                  gpointer         user_data)
{
#ifdef VERBOSE
  fprintf (stderr, "Acquired name %s\n", name);
#endif
} // on_name_acquired

static void
on_name_lost (GDBusConnection *connection,
              const gchar     *name,
              gpointer         user_data)
{
#ifdef VERBOSE
  fprintf (stderr, "Lost name %s\n", name);
#endif
  exit (1);
} // on_name_lost


// +------+-----------------------------------------------------
// | Main |
// +------+

int
main (int argc, char *argv[])
{
  guint owner_id;
  GMainLoop *loop;

  g_type_init ();

  introspection_data = g_dbus_node_info_new_for_xml (introspection_xml, NULL);
  g_assert (introspection_data != NULL);

  owner_id = g_bus_own_name (G_BUS_TYPE_SESSION,
                             "edu.grinnell.cs.glimmer.RAMStest",
                             G_BUS_NAME_OWNER_FLAGS_NONE,
                             on_bus_acquired,
                             on_name_acquired,
                             on_name_lost,
                             NULL,
                             NULL);

  // Event loop.  Wait for functions to get called asyn h.
  loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);

  g_bus_unown_name (owner_id);

  g_dbus_node_info_unref (introspection_data);

  return 0;
} // main
