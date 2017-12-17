import tkinter as tk

from serialio import Arduino
from pipettor import Targeting, Pipettor

class GuiTargeting(Targeting, tk.Frame):
    def __init__(self, pipettor, gui=None):
        super().__init__(pipettor, gui)
        self.gui = gui
        self.gui.title('Pipettor')

        self.position_tracking = False
        self.status = tk.StringVar()

        self.pack(fill=tk.BOTH, expand=tk.YES)
        self.create_widgets()

    def start(self):
        self.pipettor.setup()
        self.pipettor.arduino.start()
        self.gui.mainloop()
        self.pipettor.arduino.stop()
        self.pipettor.teardown()

    def quit(self):
        self.gui.destroy()

    # Widgets

    def create_widgets(self):
        self.create_status_widgets()
        self.quit = tk.Button(self, text='Quit', command=self.quit)
        self.quit.pack(side='bottom', fill=tk.X)
        self.create_targeting_widgets()

    def create_status_widgets(self):
        self.status_frame = tk.Frame(self)
        self.status_frame.pack(side='bottom', fill=tk.X)
        self.status_message = tk.Label(
            self.status_frame, textvariable=self.status, width=40
        )
        self.status_message.pack(fill=tk.X)

    def create_targeting_widgets(self):
        self.target_frame = tk.LabelFrame(self, text='Target Position')
        self.target_frame.pack(side='top', fill=tk.BOTH)
        self.track_target = tk.Button(self.target_frame, text='Follow Continuously',
                                      command=self.toggle_position_tracking)
        self.track_target.pack(side='bottom', fill=tk.X)
        self.apply_target = tk.Button(self.target_frame, text='Go and Hold',
                                      command=self.apply_target_position)
        self.apply_target.pack(side='bottom', fill=tk.X)
        self.target_position = tk.Scale(
            self.target_frame, length=300,
            from_=self.pipettor.top_mark, to=self.pipettor.bottom_mark,
            resolution=0.01, tickinterval=0.1, command=self.on_target_position_change,
        )
        self.target_position.set(self.pipettor.top_mark)
        self.target_position.pack(side='top', fill=tk.Y)

    # Targeting

    def on_stabilized_position(self, position_unitless, position_mL_mark):
        if not self.position_tracking:
            self.status.set('Reached and holding at the {:.2f} mL mark...'
                            .format(position_mL_mark))

    def apply_target_position(self):
        self.pipettor.set_target_mark(self.target_position.get())
        self.track_target.config(relief=tk.RAISED)
        self.status.set('Moving to the {} mL mark...'.format(self.target_position.get()))
        self.position_tracking = False

    # Target following

    def toggle_position_tracking(self):
        if self.position_tracking:
            self.position_tracking = False
            self.track_target.config(relief=tk.RAISED)
            self.status.set('No longer following target position.')
            return
        self.position_tracking = True
        self.status.set('Following target position as it is changed...'.format())
        self.track_target.config(relief=tk.SUNKEN)

    def on_target_position_change(self, new_position):
        if self.position_tracking:
            self.pipettor.set_target_mark(float(new_position))

def main():
    arduino = Arduino()
    pipettor = Pipettor(arduino)
    gui_root = tk.Tk()
    gui_app = GuiTargeting(pipettor, gui=gui_root)
    gui_app.start()


if __name__ == '__main__':
    main()

