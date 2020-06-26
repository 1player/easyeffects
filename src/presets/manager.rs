use glib;
use log::*;
use std::fs;
use std::path::Path;

pub enum PresetType {
    Input,
    Output,
}

pub struct Manager {
    input_presets_directories: Vec<String>,
    output_presets_directories: Vec<String>,
}

impl Manager {
    pub fn new() -> Manager {
        let mut input_directories = Vec::new();
        let mut output_directories = Vec::new();

        for directory in glib::get_system_config_dirs() {
            let directory_name = String::from(directory.to_str().unwrap());

            let input_dir = directory_name.clone() + "/PulseEffects/input";
            let output_dir = directory_name + "/PulseEffects/output";

            if Path::new(&input_dir).is_dir() {
                input_directories.push(input_dir);
            }

            if Path::new(&output_dir).is_dir() {
                output_directories.push(output_dir);
            }
        }

        let user_config_directory =
            String::from(glib::get_user_config_dir().unwrap().to_str().unwrap());

        let user_input_directory = user_config_directory.clone() + "/PulseEffects/input";
        let user_output_directory = user_config_directory.clone() + "/PulseEffects/output";
        let user_autoload_directory = user_config_directory + "/PulseEffects/autoload";

        fs::create_dir_all(&user_input_directory).expect("Could not create preset directory");
        fs::create_dir_all(&user_output_directory).expect("Could not create preset directory");
        fs::create_dir_all(&user_autoload_directory)
            .expect("Could not create preset autoload directory");

        input_directories.push(user_input_directory);
        output_directories.push(user_output_directory);

        input_directories.sort();
        input_directories.dedup();

        output_directories.sort();
        output_directories.dedup();

        debug!("input presets paths {:?}", input_directories);
        debug!("output presets paths {:?}", output_directories);

        Manager {
            input_presets_directories: input_directories,
            output_presets_directories: output_directories,
        }
    }

    pub fn get_names(&self, preset_type: PresetType) -> Vec<String> {
        let mut names = Vec::new();

        match preset_type {
            PresetType::Output => {
                for dir in &self.output_presets_directories {
                    let presets = get_presets_list(dir);

                    for preset in presets {
                        names.push(preset);
                    }
                }

                names.sort();
                names.dedup();

                return names;
            }
            PresetType::Input => {
                for dir in &self.input_presets_directories {
                    let presets = get_presets_list(dir);

                    for preset in presets {
                        names.push(preset);
                    }
                }

                names.sort();
                names.dedup();

                return names;
            }
        }
    }
}

fn get_presets_list(directory: &String) -> Vec<String> {
    let dir_path = Path::new(&directory);
    let mut output = Vec::new();

    if dir_path.is_dir() {
        for entry in dir_path.read_dir().expect("read_dir call failed") {
            let file_path = entry.unwrap().path();

            if file_path.is_file() {
                if file_path.extension().unwrap() == "json" {
                    let name = file_path.file_name().unwrap().to_str().unwrap();

                    output.push(String::from(name));
                }
            }
        }
    }

    return output;
}