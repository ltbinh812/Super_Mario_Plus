import os
from PIL import Image, ImageDraw, ImageFont

def stretch_button_9slice(img, new_width, new_height):
    width, height = img.size
    if new_width <= width and new_height <= height:
        return img
        
    cw = width // 2 - 2   # corner width
    ch = height // 2 - 2  # corner height
    
    # 9 slices
    # Top row
    tl = img.crop((0, 0, cw, ch))
    tm = img.crop((cw, 0, width - cw, ch))
    tr = img.crop((width - cw, 0, width, ch))
    
    # Mid row
    ml = img.crop((0, ch, cw, height - ch))
    mm = img.crop((cw, ch, width - cw, height - ch))
    mr = img.crop((width - cw, ch, width, height - ch))
    
    # Bottom row
    bl = img.crop((0, height - ch, cw, height))
    bm = img.crop((cw, height - ch, width - cw, height))
    br = img.crop((width - cw, height - ch, width, height))
    
    # Target dimensions for mid sections
    mid_w = new_width - (cw * 2)
    mid_h = new_height - (ch * 2)
    
    new_img = Image.new("RGBA", (new_width, new_height), (0, 0, 0, 0))
    
    # Paste corners
    new_img.paste(tl, (0, 0))
    new_img.paste(tr, (new_width - cw, 0))
    new_img.paste(bl, (0, new_height - ch))
    new_img.paste(br, (new_width - cw, new_height - ch))
    
    # Resize and paste edges and center
    if mid_w > 0:
        new_img.paste(tm.resize((mid_w, ch)), (cw, 0))
        new_img.paste(bm.resize((mid_w, ch)), (cw, new_height - ch))
    if mid_h > 0:
        new_img.paste(ml.resize((cw, mid_h)), (0, ch))
        new_img.paste(mr.resize((cw, mid_h)), (new_width - cw, ch))
    if mid_w > 0 and mid_h > 0:
        new_img.paste(mm.resize((mid_w, mid_h)), (cw, ch))
        
    return new_img

def create_button(base_img_path, out_path, text, font_path, font_size=16, 
                  text_color=(27, 34, 54, 255)):
    try:
        btn = Image.open(base_img_path).convert("RGBA")
        width, height = btn.size
        
        font = ImageFont.truetype(font_path, font_size)
        draw_dummy = ImageDraw.Draw(Image.new("RGBA", (1, 1)))
        bbox = draw_dummy.textbbox((0, 0), text, font=font)
        text_width = bbox[2] - bbox[0]
        text_height = bbox[3] - bbox[1]
        
        # Make ALL buttons significantly wider and taller
        min_width_needed = max(140, text_width + 40)
        min_height_needed = max(32, height + 8) # Chiều cao to hơn xíu
        
        if min_width_needed > width or min_height_needed > height:
            btn = stretch_button_9slice(btn, min_width_needed, min_height_needed)
            width = min_width_needed
            height = min_height_needed
        
        text_x = (width - text_width) // 2
        center_y_of_text = (bbox[1] + bbox[3]) / 2.0
        text_y = int(height / 2 - center_y_of_text)
        
        draw = ImageDraw.Draw(btn)
        
        # Vẽ chữ thường, không đổ bóng theo yêu cầu mới
        draw.text((text_x, text_y), text, font=font, fill=text_color)
        
        btn.save(out_path)
        print(f"Success: {out_path} (Size: {width}x{height})")
        
    except Exception as e:
        print(f"Error processing {text}: {e}")

if __name__ == "__main__":
    dir_path = os.path.dirname(os.path.abspath(__file__))
    font_file = os.path.join(dir_path, 'HomeVideo-Regular.ttf')
    
    bar_normal = os.path.join(dir_path, 'bar.png')
    bar_press = os.path.join(dir_path, 'bar_press.png')
    
    labels = ["Play", "Settings", "Exit", "Characters"]
    FONT_SIZE = 16
    
    for label in labels:
        file_suffix = label.lower().replace(" ", "_")
        
        out_normal = os.path.join(dir_path, f"btn_{file_suffix}.png")
        create_button(bar_normal, out_normal, label, font_file, FONT_SIZE)
        
        out_press = os.path.join(dir_path, f"btn_{file_suffix}_press.png")
        create_button(bar_press, out_press, label, font_file, FONT_SIZE)
