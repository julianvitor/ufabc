import rembg
from PIL import Image
import os
from tqdm import tqdm

def remove_background(input_folder, output_folder):
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)

    image_files = [f for f in os.listdir(input_folder) if f.lower().endswith(('.jpg', '.jpeg', '.png', '.gif'))]

    for image_file in tqdm(image_files, desc="Processando imagens"):
        input_path = os.path.join(input_folder, image_file)
        output_path = os.path.join(output_folder, f"sem_fundo_{image_file}")

        with open(input_path, "rb") as input_file, open(output_path, "wb") as output_file:
            input_data = input_file.read()
            output_data = rembg.remove(input_data)
            output_file.write(output_data)

    print("Processo concluído!")

if __name__ == "__main__":
    input_folder = os.getcwd()  # Obtém o diretório atual
    output_folder = os.path.join(input_folder, "sem_fundo")

    remove_background(input_folder, output_folder)
