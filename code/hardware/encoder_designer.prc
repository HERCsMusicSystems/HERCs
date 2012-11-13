import studio
import f1
import help

program encoder_designer
	[disp actual]

preprocessor := f1

[[disp] [disp 16 64 50 50 70.5 31.5 -11.5]]
[[disp *count *fraction *X *Y *x *y *shift]
	[sub *x *X *dx] [sub *Y *y *dy]
	[show [*dx *dy]]
	[tan *angle [div *dy *dx #]]
	[show *angle]
	[pow *radius 2.0 [add [mult *dx *dx #] [mult *dy *dy #] #]]
	[show *radius]
	[div [mult ^pi 2.0 #] *fraction *delta]
	[show *delta]
	[actual *count *delta *X *Y *radius *angle *shift]
]

[[actual 0 * * * * * *]]
[[actual *count *delta *X *Y *radius *angle *shift]
	[actual *X *Y *radius *angle *shift]
	[sub *count 1 *c1] [sub *angle *delta *a1]
	/ [actual *c1 *delta *X *Y *radius *a1 *shift]]

[[actual *X *Y *radius *angle *shift]
	[mult *radius [cos *angle #] *x] [mult *radius [sin *angle #] *y]
	[add *X *x *shift *locx] [sub *Y *y *ysub] [add *ysub *shift *locy]
;	[show [[add 11.5 *X *x #] [sub 11.5 *Y *y #]]]
	[show [*locx *locy]]
]


end := [[preprocessor f1] [command]] .
