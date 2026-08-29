import os
from PIL import Image, ImageDraw, ImageFont

def generate_text_image(out_path, text, font_path, font_size=40, line_spacing=10):
    try:
        font = ImageFont.truetype(font_path, font_size)
        draw_dummy = ImageDraw.Draw(Image.new("RGBA", (1, 1)))
        
        lines = text.split("\n")
        
        # Calculate max width and total height
        max_width = 0
        total_height = 0
        line_bboxes = []
        for line in lines:
            bbox = draw_dummy.textbbox((0, 0), line, font=font)
            line_bboxes.append(bbox)
            w = bbox[2] - bbox[0]
            h = bbox[3] - bbox[1]
            if w > max_width:
                max_width = w
            total_height += h
            
        total_height += line_spacing * (len(lines) - 1)
        
        # Add padding for shadow and extra safety
        padding = 10
        img_width = max_width + padding * 2
        img_height = total_height + padding * 2
        
        img = Image.new("RGBA", (int(img_width), int(img_height)), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        
        shadow_color = (255, 190, 130, 255) 
        text_color = (27, 34, 54, 255)
        shadow_offset_x = 3
        shadow_offset_y = 3
        
        current_y = padding
        for i, line in enumerate(lines):
            bbox = line_bboxes[i]
            line_width = bbox[2] - bbox[0]
            line_height = bbox[3] - bbox[1]
            
            # Center horizontally
            text_x = padding + (max_width - line_width) // 2
            
            # Shadow
            draw.text((text_x + shadow_offset_x, current_y + shadow_offset_y), 
                          line, font=font, fill=shadow_color, stroke_width=2, stroke_fill=shadow_color)
            # Text
            draw.text((text_x, current_y), 
                          line, font=font, fill=text_color, stroke_width=2, stroke_fill=text_color)
            
            current_y += line_height + line_spacing
            
        img.save(out_path)
        print(f"Success: {out_path}")
        
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    dir_path = os.path.dirname(os.path.abspath(__file__))
    font_file = os.path.join(dir_path, 'HomeVideo-Regular.ttf')
    
    generate_text_image(
        out_path=os.path.join(dir_path, 'title_text_only.png'),
        text="SUPER MARIO\nPLUS",
        font_path=font_file, 
        font_size=56, # make it large
        line_spacing=10
    )
