#ifndef OVERLAYUI_H
#define OVERLAYUI_H

class OverlayUI {
public:
    virtual ~OverlayUI() = default;
    
    // Xử lý đầu vào / click
    virtual void HandleInput() = 0;
    
    // Cập nhật logic UI
    virtual void Update(float dt) = 0;
    
    // Vẽ UI lên màn hình
    virtual void Draw() = 0;
    
    // Kiểm tra xem UI đã hoàn thành chưa
    virtual bool IsFinished() const = 0;
};

#endif
