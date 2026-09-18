#include "main_paned.h"
#include <thread>
#include <glibmm/main.h>
#include <gdkmm/pixbuf.h>

MainPaned::MainPaned() 
	: Gtk::Paned{Gtk::Orientation::HORIZONTAL},
		m_button_wallpapers{"Wallpapers"}
{
	auto pixbuf = Gdk::Pixbuf::create_from_file("gear-solid.png", 100, 100, true);
	m_gear_icon.set(pixbuf);
	m_gear_icon.set_pixel_size(15);

	set_name("main-paned");
	set_position(250);

	// adding wallpapers to le stack
	m_stack.add(m_wallpapers, "window-wallpaper");

	// actually making the sidebar window relationship . they are so cute together
	set_start_child(m_sidebar);
	set_end_child(m_stack);

	m_sidebar.set_name("sidebar");
	m_stack.set_name("stack");

	m_stack.set_hexpand(true);

	// adding le butons
	m_sidebar.append(m_header_label);
	m_sidebar.append(m_button_wallpapers);
	m_sidebar.append(m_options_container);

	m_options.set_halign(Gtk::Align::START);
	m_options.set_valign(Gtk::Align::END);
	m_options.set_vexpand(true);

	m_options_container.append(m_options);

	m_options.set_name("options");
	m_options.set_child(m_gear_icon);

	m_header_label.add_css_class("header");

	m_button_wallpapers.set_name("buttons-wallpapers");

	// le buton functionality?
	m_button_wallpapers.signal_clicked().connect([this]() {
	    m_stack.set_visible_child("window-wallpaper");

	    m_wallpapers.PopulateContainer();
	});
}