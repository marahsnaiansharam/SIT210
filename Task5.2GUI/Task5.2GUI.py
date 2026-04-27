
import tkinter as tk
from gpiozero import LED, PWMLED

# Create dictionary to store led pin assignments.
house_lights = {
    "living_room_led" : PWMLED(12), # Formerly 17
    "bathroom_led" : LED(22),
    "closet_led" : LED(27)
}

# Check input from GUI to determine currently selected light, ensures others are toggled off. 
def toggle_light():

    currently_selected = selected_light.get()

    for room_selected, led in house_lights.items():
        if room_selected == currently_selected:
            if room_selected == "living_room_led":
                led.value = living_room_brightness.get() / 100
            else:
                led.on()
        else:
            led.off()

# Updates living room led brightness from retrieved slider position
def update_living_room_brightness(value):
    if selected_light.get() == "living_room_led":
        house_lights["living_room_led"].value = float(value) / 100


# Turns off all LEDs, exits GUI.
def exit_gui():
    for led in house_lights.values():
        led.off()
    main.destroy()

# Defines tkinter window specifications
main = tk.Tk()
main.title("House Light Interface")
main.geometry("600x800")

selected_light = tk.StringVar(value="None")


# Displays buttons along single row using grid format
for i, room in enumerate(house_lights.keys()):
    tk.Radiobutton(
        main,
        text = room.replace("_", " "),
        variable = selected_light,
        value = room, 
        command = toggle_light, 
        indicatoron = 0, 
        width = 12, 
        padx = 10, 
        pady = 10
    ).grid(row = 0, column = i, padx = 5, pady = 50)


# Displays living room led slider
tk.Label(main, text = "Living Room Dimmer").grid(row=1, column=0, columnspan=3, pady=(20,0))

living_room_brightness = tk.Scale(
    main,
    from_=0,
    to=100,
    orient = tk.VERTICAL, 
    length=200,
    command=update_living_room_brightness
)

living_room_brightness.set(0) # Default value for slider

# Places living room slider into position on grid
living_room_brightness.grid(row=2, column=0, columnspan=3, pady=10)

# Defines and displays exit button offset with space from button
exit_button = tk.Button(
    main,
    text="Exit",
    command=exit_gui,
    width=12
)

exit_button.grid(row=0, column=4, padx=(50, 5), pady=50)

main.mainloop()



