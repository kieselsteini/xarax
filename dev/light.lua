local data = {}
for i = 1, 256 do
    local light = math.floor(3 + math.sin((math.pi * 2) / 255 * (i - 1)) * 32)
    if light < 1 then
        light = 1
    end
    data[i] = string.format('%2d', light)
end
for i = 1, 256, 16 do
    print('    ' .. table.concat(data, ', ', i, i + 15) .. ',')
end
