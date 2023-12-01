import os
import shutil
from datetime import datetime
from exifread import process_file

def get_image_date(file_path):
    with open(file_path, 'rb') as f:
        tags = process_file(f, stop_tag='DateTimeOriginal')
        if 'EXIF DateTimeOriginal' in tags:
            date_str = str(tags['EXIF DateTimeOriginal'])
            date_object = datetime.strptime(date_str, '%Y:%m:%d %H:%M:%S')
            return date_object
    return None

def organize_photos(input_folder, output_folder):
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)

    files = [f for f in os.listdir(input_folder) if f.lower().endswith(('.jpg', '.jpeg', '.png', '.gif'))]
    files.sort(key=lambda x: os.path.getctime(os.path.join(input_folder, x)))

    for idx, file_name in enumerate(files):
        file_path = os.path.join(input_folder, file_name)
        date_taken = get_image_date(file_path)

        if date_taken:
            new_file_name = f"{idx+1:04d}_{date_taken.strftime('%Y%m%d_%H%M%S')}.jpg"
            new_file_path = os.path.join(output_folder, new_file_name)
            shutil.copy(file_path, new_file_path)
            print(f"Copied and Renamed: {file_name} -> {new_file_name}")
        else:
            print(f"Skipped: {file_name} (No capture date information)")

if __name__ == "__main__":
    input_folder = os.getcwd()  # Obtém o diretório atual
    output_folder = os.path.join(input_folder, "organized_photos")

    organize_photos(input_folder, output_folder)
