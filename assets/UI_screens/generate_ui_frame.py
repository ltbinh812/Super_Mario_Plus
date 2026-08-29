import os
from PIL import Image, ImageDraw, ImageFont

# ==============================================================================
# HƯỚNG DẪN SỬ DỤNG SCRIPT TẠO KHUNG VIỀN CÓ CHỮ ĐỨT ĐOẠN (UI TITLE)
# ==============================================================================
def generate_titled_bar(bar_path, out_path, title_text, font_path, font_size=20, 
                        margin=15, frame_thickness=2, text_y_offset=4, letter_spacing=4):
    try:
        bar = Image.open(bar_path).convert("RGBA")
        width, height = bar.size
        
        frame_layer = Image.new("RGBA", (width, height), (0, 0, 0, 0))
        draw_frame = ImageDraw.Draw(frame_layer)
        frame_color = (27, 34, 54, 255)
        
        for i in range(frame_thickness):
            rect = [margin + i, margin + i, width - 1 - margin - i, height - 1 - margin - i]
            draw_frame.rectangle(rect, outline=frame_color)
            
        font = ImageFont.truetype(font_path, font_size)
        draw_dummy = ImageDraw.Draw(Image.new("RGBA", (1, 1)))
        
        # Tính chiều cao chữ (dựa vào toàn bộ chuỗi để lấy height chuẩn)
        bbox = draw_dummy.textbbox((0, 0), title_text, font=font, stroke_width=1)
        text_height = bbox[3] - bbox[1]
        
        # Tính tổng chiều dài chữ CÓ BAO GỒM letter_spacing
        total_text_width = 0
        char_widths = []
        for char in title_text:
            w = draw_dummy.textlength(char, font=font)
            char_widths.append(w)
            total_text_width += w
            
        total_text_width += letter_spacing * (len(title_text) - 1)
        
        # Tọa độ căn giữa
        text_x = (width - total_text_width) // 2
        center_y_of_text = (bbox[1] + bbox[3]) / 2.0
        text_y = int(margin - center_y_of_text) + text_y_offset 
        
        # Cắt viền
        padding = 16 
        cutout_width = total_text_width + (padding * 2)
        cutout = Image.new("RGBA", (int(cutout_width), frame_thickness + 4), (0, 0, 0, 0))
        cutout_x = text_x - padding
        frame_layer.paste(cutout, (int(cutout_x), margin - 2))
        bar.paste(frame_layer, (0, 0), frame_layer)
        
        # Vẽ chữ từng kí tự để tạo khoảng cách (letter_spacing)
        draw_bar = ImageDraw.Draw(bar)
        shadow_color = (255, 190, 130, 255) 
        text_color = (27, 34, 54, 255)
        shadow_offset_x = 2
        shadow_offset_y = 2
        
        current_x = text_x
        for i, char in enumerate(title_text):
            # Vẽ shadow
            draw_bar.text((current_x + shadow_offset_x, text_y + shadow_offset_y), 
                          char, font=font, fill=shadow_color, stroke_width=1, stroke_fill=shadow_color)
            # Vẽ text
            draw_bar.text((current_x, text_y), 
                          char, font=font, fill=text_color, stroke_width=1, stroke_fill=text_color)
            
            # Tiến tới vị trí chữ tiếp theo
            current_x += char_widths[i] + letter_spacing
            
        bar.save(out_path)
        print(f"Success: {out_path} (Letter spacing: {letter_spacing})")
        
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    dir_path = os.path.dirname(os.path.abspath(__file__))
    font_file = os.path.join(dir_path, 'HomeVideo-Regular.ttf')
    
    generate_titled_bar(
        bar_path=os.path.join(dir_path, 'large_bar_1.png'),
        out_path=os.path.join(dir_path, 'large_bar_1_with_title.png'),
        title_text="INVENTORY",
        font_path=font_file, font_size=20, margin=15, frame_thickness=2, text_y_offset=4,
        letter_spacing=4 # Giãn cách chữ
    )
    generate_titled_bar(
        bar_path=os.path.join(dir_path, 'large_bar_2.png'),
        out_path=os.path.join(dir_path, 'large_bar_2_with_title.png'),
        title_text="SETTINGS",
        font_path=font_file, font_size=20, margin=15, frame_thickness=2, text_y_offset=4,
        letter_spacing=4
    )
    
    # -------------------------------------------------------------------------
    # GENERATE TITLE
    # -------------------------------------------------------------------------
    def generate_multiline_title(bar_path, out_path, lines, font_path, font_size=32, text_y_offset=0, margin=15, frame_thickness=2):
        try:
            bar = Image.open(bar_path).convert("RGBA")
            width, height = bar.size
            
            # Draw frame
            frame_layer = Image.new("RGBA", (width, height), (0, 0, 0, 0))
            draw_frame = ImageDraw.Draw(frame_layer)
            frame_color = (27, 34, 54, 255)
            
            for i in range(frame_thickness):
                rect = [margin + i, margin + i, width - 1 - margin - i, height - 1 - margin - i]
                draw_frame.rectangle(rect, outline=frame_color)
                
            bar.paste(frame_layer, (0, 0), frame_layer)
            
            font = ImageFont.truetype(font_path, font_size)
            draw_dummy = ImageDraw.Draw(Image.new("RGBA", (1, 1)))
            
            total_text_height = 0
            line_bboxes = []
            for line in lines:
                bbox = draw_dummy.textbbox((0, 0), line, font=font)
                line_bboxes.append(bbox)
                total_text_height += (bbox[3] - bbox[1])
                
            spacing = 10
            total_text_height += spacing * (len(lines) - 1)
            
            start_y = (height - total_text_height) // 2 + text_y_offset
            
            draw_bar = ImageDraw.Draw(bar)
            shadow_color = (255, 190, 130, 255) 
            text_color = (27, 34, 54, 255)
            shadow_offset_x = 2
            shadow_offset_y = 2
            
            current_y = start_y
            for i, line in enumerate(lines):
                bbox = line_bboxes[i]
                line_width = bbox[2] - bbox[0]
                line_height = bbox[3] - bbox[1]
                
                text_x = (width - line_width) // 2
                
                # Shadow
                draw_bar.text((text_x + shadow_offset_x, current_y + shadow_offset_y), 
                              line, font=font, fill=shadow_color, stroke_width=1, stroke_fill=shadow_color)
                # Text
                draw_bar.text((text_x, current_y), 
                              line, font=font, fill=text_color, stroke_width=1, stroke_fill=text_color)
                
                current_y += line_height + spacing
                
            bar.save(out_path)
            print(f"Success: {out_path}")
        except Exception as e:
            print(f"Error: {e}")

    generate_multiline_title(
        bar_path=os.path.join(dir_path, 'large_bar_1.png'),
        out_path=os.path.join(dir_path, 'title_super_mario_plus.png'),
        lines=["SUPER", "MARIO", "PLUS"],
        font_path=font_file, 
        font_size=40,
        text_y_offset=-10,
        margin=15,
        frame_thickness=2
    )
