from pipettor import Targeting, Pipettor

class BatchTargeting(Targeting):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.targets = [
            self.pipettor.bottom_mark,
            self.pipettor.top_mark,
            0.5,  # mL mark
            0.25,  # mL mark
            self.pipettor.bottom_mark
        ]
        self.current_step = 0

    def on_stabilized_position(self, position_unitless, position_mL_mark):
        try:
            next_target = self.targets[self.current_step]
        except IndexError:
            print('Finished the batch sequence!')
            self.pipettor.running = False
            return
        input('Press enter to move the pipette to the {:.2f} mark: '.format(next_target))
        self.pipettor.set_target_mark(next_target)
        self.current_step += 1

def main():
    pipettor = Pipettor()
    targeting = BatchTargeting(pipettor)
    pipettor.setup()
    pipettor.start()


if __name__ == '__main__':
    main()

