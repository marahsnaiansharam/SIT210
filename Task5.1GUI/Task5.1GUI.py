
import tkinter as tk
from gpiozero import LED

# Create dictionary to store led pin assignments.
house_lights = {
    "living_room_led" : LED(17),
    "bathroom_led" : LED(22),
    "closet_led" : LED(27)
}

# Check input from GUI to determine currently selected light, ensures others are toggled off. 
def toggle_light():

    currently_selected = selected_light.get()

    for room_selected, led in house_lights.items():
        if room_selected == currently_selected:
            led.on()
        else:
            led.off()

# Turns off all LEDs, exits GUI.
def exit_gui():
    for led in house_lights.values():
        led.off()
    main.destroy()

# Defines tkinter window specifications
main = tk.Tk()
main.title("House Light Interface")
main.geometry("600x200")

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

# Defines and displays exit button offset with space from button
exit_button = tk.Button(
    main,
    text="Exit",
    command=exit_gui,
    width=12
)

exit_button.grid(row=0, column=4, padx=(50, 5), pady=50)

main.mainloop()



