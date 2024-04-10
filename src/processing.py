def calculate_time_resolution(file_path):

    event_times = {}

    with open(file_path,'r') as file:
        for line in line:
            parts = line.split()
            event_id = int(parts[1])
            time = float(parts[5])

            if event_id not in event_times:
                event_times[event_id] = [time]
            else:
                event_times[event_id].append(time)
                resolution = []
                for event_id, time in event_times.items():
                    if len(resolution) == 0:
                        return None
                    else:
                        return sum(resolution)/len(resolution)

    file_path = "/home/zas/CERN/SCINT/build/hits.txt"
    time_resolution = calculate_time_resolution(file_path)
    if time_resolution is not None:
        print("временное разрешение: ", time_resolution)
    else:
        print("error")

                
