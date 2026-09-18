#include "wallpapers.h"
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/image.h>
#include <gtkmm/button.h>
#include <gdkmm/pixbuf.h>
#include <filesystem>
#include <iostream>
#include <cstdlib>
#include <glibmm.h>
#include <thread>
#include <vector>
#include <utility>

Wallpapers::Wallpapers()
	: Gtk::Box{Gtk::Orientation::VERTICAL},
		m_label{"Select a wallpaper."}
{
	set_name("wallpapers");

    set_halign(Gtk::Align::FILL);

	m_content.set_orientation(Gtk::Orientation::VERTICAL);
	m_content.set_vexpand(true);
	m_container.set_child(m_content);

	m_content.set_name("wallpapers-content");
	m_container.set_name("wallpapers-container");
	m_label.set_name("wallpapers-label");
    m_label.add_css_class("header");

	append(m_label);
	append(m_container);
}

void Wallpapers::PopulateContainer()
{
    if (wallpaper_folder == "" || result_folder == "" || IsLoading()) {
        std::cout << "bye";
        return;
    }

    SetLoading(true);

    for (auto const& old_entry : m_content.get_children()) {
        old_entry->unparent();
    }

    auto* loading_label = Gtk::manage(new Gtk::Label("Loading wallpapers... Please wait"));
    loading_label->set_name("loading-label");
    m_content.append(*loading_label);

    std::thread([this, loading_label]() {
        try {
            std::vector<WallpaperItem> items;

            if (std::filesystem::exists(wallpaper_folder)) {
                for (auto const& entry : std::filesystem::directory_iterator(wallpaper_folder)) {
                    try {
                        if (entry.is_regular_file()) {
                            auto pixbuf = Gdk::Pixbuf::create_from_file(entry.path().string(), 100, 100, true);
                            items.push_back({entry.path().filename().string(), pixbuf});
                        }
                    } catch (...) {
                        // Skip unreadable files safely
                    }
                }
            }

            Glib::signal_idle().connect_once([this, items = std::move(items), loading_label]() {
                for (auto const& item : items) {
                    auto* entry_box = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
                    entry_box->add_css_class("wallpaper-entry");

                    auto* name_label = Gtk::manage(new Gtk::Label(item.name));
                    name_label->set_halign(Gtk::Align::CENTER);
                    name_label->set_valign(Gtk::Align::CENTER);
                    name_label->set_hexpand(true);

                    auto* image = Gtk::manage(new Gtk::Image(item.pixbuf));
                    image->set_halign(Gtk::Align::START);
                    image->set_valign(Gtk::Align::CENTER);
                    image->set_pixel_size(100);

                    auto* button = Gtk::manage(new Gtk::Button("Apply"));
                    button->set_halign(Gtk::Align::END);
                    button->set_valign(Gtk::Align::CENTER);

                    entry_box->append(*image);
                    entry_box->append(*name_label);
                    entry_box->append(*button);
                    m_content.append(*entry_box);
                }

                loading_label->unparent();
                SetLoading(false);
            });

        } catch (const std::exception& e) {
            std::cerr << "Error loading wallpapers: " << e.what() << std::endl;
            
            Glib::signal_idle().connect_once([this, loading_label]() {
                loading_label->unparent();
                SetLoading(false);
            });
        }
    }).detach();
}

bool Wallpapers::IsLoading() const {
	return is_loading;
}

void Wallpapers::SetLoading(bool toggle){
	is_loading = toggle;
}