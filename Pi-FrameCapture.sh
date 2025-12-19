# Setup output directory for captured frames
OUTDIR="$HOME/Projects/moss/frames"
mkdir -p "$OUTDIR"

echo 1 | sudo tee /sys/module/imx296/parameters/trigger_mode >/dev/null # Disable hardware trigger mode

# user assigned variables
exposures=(100 200 300 400 500 600 700 800 900 1000) # Exposure times in microseconds
gain=5
# end of user assigned variables

time_array_size=${#exposures[@]}
current_time=0 # Index into exposures array

# Capture loop: cycles through exposure times
while [ "$current_time" -lt "$time_array_size" ]; do
    exp_us=${exposures[$current_time]}

    filename="$OUTDIR/T_${exp_us}_G_${gain}.jpg"
    rpicam-still --immediate -n --shutter "$exp_us" --gain "$gain" --awb auto -o "$filename"
    echo "Captured frame: $filename"

    current_time=$((current_time + 1))
done

echo "Capture sequence completed: $current_time frames captured."

# chmod +x cap.sh
# ./cap.sh

# scp -r danie@raspberrypi.local:~/Projects/moss/frames "C:\Users\danie\OneDrive\Desktop\Camera Output\frames"
