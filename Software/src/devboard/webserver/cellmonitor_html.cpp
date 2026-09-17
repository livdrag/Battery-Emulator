#include "cellmonitor_html.h"
#include <Arduino.h>
#include "../../battery/BATTERIES.h"
#include "../../datalayer/datalayer.h"

//Legend label for the cyan bars. A BMS can flag cells for balancing long before it actually bleeds
//them, so when the aggregate status reports that waiting state, label the bars as pending instead.
static const char* balancing_legend_label(balancing_status_enum status) {
  return (status == BALANCING_STATUS_BLOCKED) ? "Pending" : "Balancing";
}

String cellmonitor_processor(const String& var) {
  if (var == "X") {
    String content = "";
    // Page formatH
    content += "<style>";
    content += "body { background-color: black; color: white; }";
    content +=
        "button { background-color: #505E67; color: white; border: none; padding: 10px 20px; margin-bottom: 20px; "
        "cursor: pointer; border-radius: 10px; }";
    content += "button:hover { background-color: #3A4A52; }";
    content += ".container { display: flex; flex-wrap: wrap; justify-content: space-around; }";
    content += ".cell { padding: 10px; border: 1px solid white; text-align: center; }";
    content += ".low-voltage { color: red; }";              // Style for low voltage text
    content += ".voltage-values { margin-bottom: 10px; }";  // Style for voltage values section

    if (battery3) {
      content +=
          "#graph, #graph2, #graph3, #graphCan {display: flex;align-items: flex-end;height: 200px;border: 1px solid "
          "#ccc;position: "
          "relative;}";
    } else if (battery2) {
      content +=
          "#graph, #graph2, #graphCan {display: flex;align-items: flex-end;height: 200px;border: 1px solid "
          "#ccc;position: "
          "relative;}";
    } else {
      content +=
          "#graph, #graphCan {display: flex;align-items: flex-end;height: 200px;border: 1px solid #ccc;position: "
          "relative;}";
    }
    content +=
        ".bar {margin: 0 0px;background-color: blue;display: inline-block;position: relative;cursor: pointer;border: "
        "1px solid white; /* Add this line */}";
    content += ".bar-can {background-color: #2E8B57;}";  // Distinct color for the 0x215-sourced graph

    if (battery3) {
      content +=
          "#valueDisplay, #valueDisplay2, #valueDisplay3, #valueDisplayCan {text-align: left;font-weight: "
          "bold;margin-top: 10px;}";
    } else if (battery2) {
      content +=
          "#valueDisplay, #valueDisplay2, #valueDisplayCan {text-align: left;font-weight: bold;margin-top: 10px;}";
    } else {
      content += "#valueDisplay, #valueDisplayCan {text-align: left;font-weight: bold;margin-top: 10px;}";
    }
    content += "</style>";

    content += "<button onclick='home()'>Back to main page</button>";

    // Start a new block with a specific background color
    content += "<div style='background-color: #303E47; padding: 10px; margin-bottom: 10px; border-radius: 50px'>";

    // Display max, min, and deviation voltage values
    content += "<div id='voltageValues' class='voltage-values'></div>";
    // Display cells (each cell shows UDS mV / 0x215 mV)
    content += "<div id='cellContainer' class='container'></div>";
    // Display bars (UDS-sourced, primary graph)
    content += "<div id='graph'></div>";
    // Display single hovered value
    content += "<div id='valueDisplay'>Value: ...</div>";
    //Legend for graph
    content +=
        "<span style='color: white; background-color: blue; font-weight: bold; padding: 2px 8px; border-radius: 4px; "
        "margin-right: 15px;'>Idle</span>";
    bool battery_balancing = false;
    // Check per-cell balancing status
    for (uint8_t i = 0u; i < datalayer.battery.info.number_of_cells; i++) {
      battery_balancing = datalayer.battery.status.cell_balancing_status[i];
      if (battery_balancing)
        break;
    }
    if (battery_balancing) {
      content +=
          "<span style='color: black; background-color: #00FFFF; font-weight: bold; padding: 2px 8px; border-radius: "
          "4px; margin-right: 15px;'>";
      content += balancing_legend_label(datalayer.battery.status.balancing_status);
      content += "</span>";
    }
    // Also check overall balancing status enum (for batteries without per-cell data)
    else if (datalayer.battery.status.balancing_status == BALANCING_STATUS_ACTIVE) {
      content +=
          "<span style='color: black; background-color: #ff9900ff; font-weight: bold; padding: 2px 8px; border-radius: "
          "4px; margin-right: 15px;'>Balancing is active now!</span>";
    }
    content +=
        "<span style='color: white; background-color: red; font-weight: bold; padding: 2px 8px; border-radius: "
        "4px;'>Min/Max</span>";

    // --- Second graph: higher-resolution cell voltages sourced from the 0x215 CAN broadcast ---
    content += "<h4 style='margin-top:20px;margin-bottom:5px;'>0x215 cell voltages (higher resolution)</h4>";
    content += "<div id='voltageValuesCan' class='voltage-values'></div>";
    content += "<div id='graphCan'></div>";
    content += "<div id='valueDisplayCan'>Value: ...</div>";

    // Close the block
    content += "</div>";

    if (battery2) {
      // Start a new block with a specific background color
      content += "<div style='background-color: #303E41; padding: 10px; margin-bottom: 10px; border-radius: 50px'>";

      // Display max, min, and deviation voltage values
      content += "<div id='voltageValues2' class='voltage-values'></div>";
      // Display cells
      content += "<div id='cellContainer2' class='container'></div>";
      // Display bars
      content += "<div id='graph2'></div>";
      // Display single hovered value
      content += "<div id='valueDisplay2'>Value: ...</div>";
      //Legend for graph
      content +=
          "<span style='color: white; background-color: blue; font-weight: bold; padding: 2px 8px; border-radius: 4px; "
          "margin-right: 15px;'>Idle</span>";

      bool battery2_balancing = false;
      for (uint8_t i = 0u; i < datalayer.battery2.info.number_of_cells; i++) {
        battery2_balancing = datalayer.battery2.status.cell_balancing_status[i];
        if (battery2_balancing)
          break;
      }
      if (battery2_balancing) {
        content +=
            "<span style='color: black; background-color: #00FFFF; font-weight: bold; padding: 2px 8px; border-radius: "
            "4px; margin-right: 15px;'>";
        content += balancing_legend_label(datalayer.battery2.status.balancing_status);
        content += "</span>";
      }
      content +=
          "<span style='color: white; background-color: red; font-weight: bold; padding: 2px 8px; border-radius: "
          "4px;'>Min/Max</span>";

      // Close the block
      content += "</div>";
    }

    if (battery3) {
      // Start a new block with a specific background color
      content += "<div style='background-color: #313e41ff; padding: 10px; margin-bottom: 10px; border-radius: 50px'>";

      // Display max, min, and deviation voltage values
      content += "<div id='voltageValues3' class='voltage-values'></div>";
      // Display cells
      content += "<div id='cellContainer3' class='container'></div>";
      // Display bars
      content += "<div id='graph3'></div>";
      // Display single hovered value
      content += "<div id='valueDisplay3'>Value: ...</div>";
      //Legend for graph
      content +=
          "<span style='color: white; background-color: blue; font-weight: bold; padding: 2px 8px; border-radius: 4px; "
          "margin-right: 15px;'>Idle</span>";

      bool battery3_balancing = false;
      for (uint8_t i = 0u; i < datalayer.battery3.info.number_of_cells; i++) {
        battery3_balancing = datalayer.battery3.status.cell_balancing_status[i];
        if (battery3_balancing)
          break;
      }
      if (battery3_balancing) {
        content +=
            "<span style='color: black; background-color: #00FFFF; font-weight: bold; padding: 2px 8px; border-radius: "
            "4px; margin-right: 15px;'>";
        content += balancing_legend_label(datalayer.battery3.status.balancing_status);
        content += "</span>";
      }
      content +=
          "<span style='color: white; background-color: red; font-weight: bold; padding: 2px 8px; border-radius: "
          "4px;'>Min/Max</span>";

      // Close the block
      content += "</div>";
    }

    content += "<button onclick='home()'>Back to main page</button>";

    content += "<script>";

    // --- Primary battery: UDS-sourced values (drives the main graph + cell grid, unchanged filtering) ---
    // --- and the parallel 0x215-sourced value for the SAME cell index, for the "uds/215" label ---
    // `data`, `balancing`, and `dataCanPaired` are all built together below so their indices line up.
    {
      String dataArr = "[";
      String balancingArr = "[";
      String canPairedArr = "[";
      for (uint8_t i = 0u; i < datalayer.battery.info.number_of_cells; i++) {
        if (datalayer.battery.status.cell_voltages_mV[i] == 0) {
          continue;
        }
        dataArr += String(datalayer.battery.status.cell_voltages_mV[i]) + ",";
        balancingArr += datalayer.battery.status.cell_balancing_status[i] ? "true," : "false,";
        // May legitimately be 0 if the 0x215 handler hasn't populated this cell index yet -
        // the JS below renders that as "-" rather than a bogus 0 mV reading.
        canPairedArr += String(datalayer.battery.status.cell_voltages_mV_215[i]) + ",";
      }
      dataArr += "]";
      balancingArr += "]";
      canPairedArr += "]";

      content += "const data = " + dataArr + ";";
      content += "const balancing = " + balancingArr + ";";
      content += "const dataCanPaired = " + canPairedArr + ";";
    }

    // --- Second, independent series: 0x215-sourced values, filtered on THEIR OWN zero check,
    //     used only to draw the second graph (may have a different populated set than UDS). ---
    {
      String canArr = "[";
      String canBalancingArr = "[";
      for (uint8_t i = 0u; i < datalayer.battery.info.number_of_cells; i++) {
        if (datalayer.battery.status.cell_voltages_mV_215[i] == 0) {
          continue;
        }
        canArr += String(datalayer.battery.status.cell_voltages_mV_215[i]) + ",";
        canBalancingArr += datalayer.battery.status.cell_balancing_status[i] ? "true," : "false,";
      }
      canArr += "]";
      canBalancingArr += "]";

      content += "const dataCan = " + canArr + ";";
      content += "const balancingCan = " + canBalancingArr + ";";
    }

    content += "const min_mv = Math.min(...data) - 20;";
    content += "const max_mv = Math.max(...data) + 20;";
    content += "const min_index = data.indexOf(Math.min(...data));";
    content += "const max_index = data.indexOf(Math.max(...data));";
    content += "const graphContainer = document.getElementById('graph');";
    content += "const valueDisplay = document.getElementById('valueDisplay');";
    content += "const cellContainer = document.getElementById('cellContainer');";

    content += "const min_mvCan = dataCan.length ? Math.min(...dataCan) - 20 : 0;";
    content += "const max_mvCan = dataCan.length ? Math.max(...dataCan) + 20 : 0;";
    content += "const min_indexCan = dataCan.indexOf(Math.min(...dataCan));";
    content += "const max_indexCan = dataCan.indexOf(Math.max(...dataCan));";
    content += "const graphContainerCan = document.getElementById('graphCan');";
    content += "const valueDisplayCan = document.getElementById('valueDisplayCan');";

    content += "function home() { window.location.href = '/'; }";

    // Arduino-style map() function
    content +=
        "function map(value, fromLow, fromHigh, toLow, toHigh) {return (value - fromLow) * (toHigh - toLow) / "
        "(fromHigh - fromLow) + toLow;}";

    // Mark cell and bar with highest/lowest values
    content +=
        "function checkMinMax(cell, bar, index) {if ((index == min_index) || (index == max_index)) "
        "{cell.style.borderColor = 'red';bar.style.borderColor = 'red';}}";
    content +=
        "function checkMinMaxCan(bar, index) {if ((index == min_indexCan) || (index == max_indexCan)) "
        "{bar.style.borderColor = 'red';}}";

    // Bar function. Basically get the mV, scale the height and add a bar div to its container
    content +=
        "function createBars(data) {"
        "data.forEach((mV, index) => {"
        "const bar = document.createElement('div');"
        "const mV_limited = map(mV, min_mv, max_mv, 20, 200);"
        "bar.className = 'bar';"
        "bar.id = `barIndex${index}`;"
        "bar.style.height = `${mV_limited}px`;"
        "bar.style.width = `${750/data.length}px`;"
        "if (balancing[index]) {"
        "  bar.style.backgroundColor = '#00FFFF';"  // Cyan color for balancing
        "  bar.style.borderColor = '#00FFFF';"
        "} else {"
        "  bar.style.backgroundColor = 'blue';"  // Normal blue for non-balancing
        "  bar.style.borderColor = 'white';"
        "}"

        "const cell = document.getElementById(`cellIndex${index}`);"

        "checkMinMax(cell, bar, index);"

        "bar.addEventListener('mouseenter', () => {"
        "    valueDisplay.textContent = `Value: ${mV}` + (balancing[index] ? ' (balancing)' : '');"
        "    bar.style.backgroundColor = balancing[index] ? '#80FFFF' : 'lightblue';"
        "    cell.style.backgroundColor = balancing[index] ? '#006666' : 'blue';"
        "});"

        "bar.addEventListener('mouseleave', () => {"
        "valueDisplay.textContent = 'Value: ...';"
        "bar.style.backgroundColor = balancing[index] ? '#00FFFF' : 'blue';"  // Restore cyan if balancing, else blue
        "cell.style.removeProperty('background-color');"
        "});"

        "graphContainer.appendChild(bar);"
        "});"
        "}";

    // Bar function for the 0x215-sourced (more accurate) graph
    content +=
        "function createBarsCan(dataCan) {"
        "if (dataCan.length === 0) { return; }"
        "dataCan.forEach((mV, index) => {"
        "const bar = document.createElement('div');"
        "const mV_limited = map(mV, min_mvCan, max_mvCan, 20, 200);"
        "bar.className = 'bar bar-can';"
        "bar.id = `barCanIndex${index}`;"
        "bar.style.height = `${mV_limited}px`;"
        "bar.style.width = `${750/dataCan.length}px`;"
        "if (balancingCan[index]) {"
        "  bar.style.backgroundColor = '#00FFFF';"
        "  bar.style.borderColor = '#00FFFF';"
        "} else {"
        "  bar.style.borderColor = 'white';"
        "}"

        "checkMinMaxCan(bar, index);"

        "bar.addEventListener('mouseenter', () => {"
        "    valueDisplayCan.textContent = `Value: ${mV}` + (balancingCan[index] ? ' (balancing)' : '');"
        "    bar.style.backgroundColor = balancingCan[index] ? '#80FFFF' : '#7BC79E';"
        "});"

        "bar.addEventListener('mouseleave', () => {"
        "valueDisplayCan.textContent = 'Value: ...';"
        "bar.style.backgroundColor = balancingCan[index] ? '#00FFFF' : '#2E8B57';"
        "});"

        "graphContainerCan.appendChild(bar);"
        "});"
        "}";

    // Cell population function. For each value, add a cell block showing UDS mV / 0x215 mV
    content +=
        "function createCells(data) {"
        "data.forEach((mV, index) => {"
        "const cell = document.createElement('div');"
        "cell.className = 'cell';"
        "cell.id = `cellIndex${index}`;"
        "const mVCan = dataCanPaired[index];"
        "const canText = (mVCan === undefined || mVCan === 0) ? '-' : mVCan;"
        "let cellContent = `Cell ${index + 1}<br>${mV} / ${canText} mV`;"
        "if (mV < 3000) {"
        "  cellContent = `<span class='low-voltage'>${cellContent}</span>`;"
        "}"
        "cell.innerHTML = cellContent;"

        "cell.addEventListener('mouseenter', () => {"
        "let bar = document.getElementById(`barIndex${index}`);"
        "valueDisplay.textContent = `Value: ${mV}`;"
        "bar.style.backgroundColor = balancing[index] ? '#80FFFF' : 'lightblue';"  // Lighter cyan if balancing
        "cell.style.backgroundColor = balancing[index] ? '#006666' : 'blue';"      // Darker cyan if balancing
        "});"

        "cell.addEventListener('mouseleave', () => {"
        "let bar = document.getElementById(`barIndex${index}`);"
        "bar.style.backgroundColor = balancing[index] ? '#00FFFF' : 'blue';"  // Restore original color
        "cell.style.removeProperty('background-color');"
        "});"

        "cellContainer.appendChild(cell);"
        "});"
        "}";

    // On fetch, update the header of max/min/deviation client-side for consistency
    content +=
        "function updateVoltageValues(data) {"
        "const min_mv = Math.min(...data);"
        "const max_mv = Math.max(...data);"
        "const cell_dev = max_mv - min_mv;"
        "const voltVal = document.getElementById('voltageValues');"
        "voltVal.innerHTML = `Max Voltage : ${max_mv} mV<br>Min Voltage: ${min_mv} mV<br>Voltage Deviation: ";
    if (datalayer.battery.status.balancing_status == BALANCING_STATUS_ACTIVE) {
      content += "${cell_dev} mV (Battery is balancing now!)`}";
    } else {
      content += "${cell_dev} mV`}";
    }

    content +=
        "function updateVoltageValuesCan(dataCan) {"
        "const voltValCan = document.getElementById('voltageValuesCan');"
        "if (dataCan.length === 0) {"
        "  voltValCan.textContent = '0x215 cell voltages not yet received';"
        "  return;"
        "}"
        "const min_mv = Math.min(...dataCan);"
        "const max_mv = Math.max(...dataCan);"
        "const cell_dev = max_mv - min_mv;"
        "voltValCan.innerHTML = `Max Voltage : ${max_mv} mV<br>Min Voltage: ${min_mv} mV<br>Voltage Deviation: "
        "${cell_dev} mV`;"
        "}";

    // If we have values, do the thing. Otherwise, display friendly message and wait
    content += "if (data.length != 0) {";
    content += "createCells(data);";
    content += "createBars(data);";
    content += "updateVoltageValues(data);";
    content += "}";
    content += "else {";
    if (datalayer.battery.info.number_of_cells > 0) {
      content += "document.getElementById('voltageValues').textContent = '" +
                 String(datalayer.battery.info.number_of_cells) + " cells configured, but cellvoltages not yet read';";
    } else {
      content +=
          "document.getElementById('voltageValues').textContent = 'Amount of cells unknown. Cellvoltages not yet "
          "read';";
    }
    content += "}";

    content += "createBarsCan(dataCan);";
    content += "updateVoltageValuesCan(dataCan);";

    if (battery2) {
      // Populate cell data
      content += "const data2 = [";
      for (uint8_t i = 0u; i < datalayer.battery2.info.number_of_cells; i++) {
        if (datalayer.battery2.status.cell_voltages_mV[i] == 0) {
          continue;
        }
        content += String(datalayer.battery2.status.cell_voltages_mV[i]) + ",";
      }
      content += "];";

      content += "const balancing2 = [";
      for (uint8_t i = 0u; i < datalayer.battery2.info.number_of_cells; i++) {
        if (datalayer.battery2.status.cell_voltages_mV[i] == 0) {
          continue;
        }
        content += datalayer.battery2.status.cell_balancing_status[i] ? "true," : "false,";
      }
      content += "];";

      content += "const min_mv2 = Math.min(...data2) - 20;";
      content += "const max_mv2 = Math.max(...data2) + 20;";
      content += "const min_index2 = data2.indexOf(Math.min(...data2));";
      content += "const max_index2 = data2.indexOf(Math.max(...data2));";
      content += "const graphContainer2 = document.getElementById('graph2');";
      content += "const valueDisplay2 = document.getElementById('valueDisplay2');";
      content += "const cellContainer2 = document.getElementById('cellContainer2');";

      // Arduino-style map() function
      content +=
          "function map2(value, fromLow, fromHigh, toLow, toHigh) {return (value - fromLow) * (toHigh - toLow) / "
          "(fromHigh - fromLow) + toLow;}";

      // Mark cell and bar with highest/lowest values
      content +=
          "function checkMinMax2(cell2, bar2, index2) {if ((index2 == min_index2) || (index2 == max_index2)) "
          "{cell2.style.borderColor = 'red';bar2.style.borderColor = 'red';}}";

      // Bar function. Basically get the mV, scale the height and add a bar div to its container
      content +=
          "function createBars2(data2) {"
          "data2.forEach((mV, index2) => {"
          "const bar2 = document.createElement('div');"
          "const mV_limited2 = map2(mV, min_mv2, max_mv2, 20, 200);"
          "bar2.className = 'bar';"
          "bar2.id = `barIndex2${index2}`;"
          "bar2.style.height = `${mV_limited2}px`;"
          "bar2.style.width = `${750/data2.length}px`;"
          "if (balancing2[index2]) {"
          "  bar2.style.backgroundColor = '#00FFFF';"  // Cyan color for balancing
          "  bar2.style.borderColor = '#00FFFF';"
          "} else {"
          "  bar2.style.backgroundColor = 'blue';"  // Normal blue for non-balancing
          "  bar2.style.borderColor = 'white';"
          "}"
          "const cell2 = document.getElementById(`cellIndex2${index2}`);"

          "checkMinMax2(cell2, bar2, index2);"

          "bar2.addEventListener('mouseenter', () => {"
          "    valueDisplay2.textContent = `Value: ${mV}` + (balancing[index2] ? ' (balancing)' : '');"
          "    bar2.style.backgroundColor = balancing2[index2] ? '#80FFFF' : 'lightblue';"
          "    cell2.style.backgroundColor = balancing2[index2] ? '#006666' : 'blue';"
          "});"

          "bar2.addEventListener('mouseleave', () => {"
          "valueDisplay2.textContent = 'Value: ...';"
          "bar2.style.backgroundColor = balancing2[index2] ? '#00FFFF' : 'blue';"  // Restore cyan if balancing, else blue
          "cell2.style.removeProperty('background-color');"
          "});"

          "graphContainer2.appendChild(bar2);"
          "});"
          "}";

      // Cell population function. For each value, add a cell block with its value
      content +=
          "function createCells2(data2) {"
          "data2.forEach((mV, index2) => {"
          "const cell2 = document.createElement('div');"
          "cell2.className = 'cell';"
          "cell2.id = `cellIndex2${index2}`;"
          "let cellContent2 = `Cell ${index2 + 1}<br>${mV} mV`;"
          "if (mV < 3000) {"
          "cellContent2 = `<span class='low-voltage'>${cellContent2}</span>`;"
          "}"
          "cell2.innerHTML = cellContent2;"

          "cell2.addEventListener('mouseenter', () => {"
          "let bar2 = document.getElementById(`barIndex2${index2}`);"
          "valueDisplay2.textContent = `Value: ${mV}`;"
          "bar2.style.backgroundColor = balancing2[index2] ? '#80FFFF' : 'lightblue';"  // Lighter cyan if balancing
          "cell2.style.backgroundColor = balancing2[index2] ? '#006666' : 'blue';"      // Darker cyan if balancing
          "});"

          "cell2.addEventListener('mouseleave', () => {"
          "let bar2 = document.getElementById(`barIndex2${index2}`);"
          "bar2.style.backgroundColor = balancing2[index2] ? '#00FFFF' : 'blue';"  // Restore original color
          "cell2.style.removeProperty('background-color');"
          "});"

          "cellContainer2.appendChild(cell2);"
          "});"
          "}";

      // On fetch, update the header of max/min/deviation client-side for consistency
      content +=
          "function updateVoltageValues2(data2) {"
          "const min_mv2 = Math.min(...data2);"
          "const max_mv2 = Math.max(...data2);"
          "const cell_dev2 = max_mv2 - min_mv2;"
          "const voltVal2 = document.getElementById('voltageValues2');"
          "voltVal2.innerHTML = `Battery #2<br>Max Voltage : ${max_mv2} mV<br>Min Voltage: ${min_mv2} mV<br>Voltage "
          "Deviation: "
          "${cell_dev2} mV`"
          "}";

      // If we have values, do the thing. Otherwise, display friendly message and wait
      content += "if (data2.length != 0) {";
      content += "createCells2(data2);";
      content += "createBars2(data2);";
      content += "updateVoltageValues2(data2);";
      content += "}";
      content += "else {";
      if (datalayer.battery2.info.number_of_cells > 0) {
        content += "document.getElementById('voltageValues2').textContent = '" +
                   String(datalayer.battery2.info.number_of_cells) +
                   " cells configured, but cellvoltages not yet read';";
      } else {
        content +=
            "document.getElementById('voltageValues2').textContent = 'Amount of cells unknown. Cellvoltages not yet "
            "read';";
      }
      content += "}";
    }

    if (battery3) {
      // Populate cell data
      content += "const data3 = [";
      for (uint8_t i = 0u; i < datalayer.battery3.info.number_of_cells; i++) {
        if (datalayer.battery3.status.cell_voltages_mV[i] == 0) {
          continue;
        }
        content += String(datalayer.battery3.status.cell_voltages_mV[i]) + ",";
      }
      content += "];";

      content += "const balancing3 = [";
      for (uint8_t i = 0u; i < datalayer.battery3.info.number_of_cells; i++) {
        if (datalayer.battery3.status.cell_voltages_mV[i] == 0) {
          continue;
        }
        content += datalayer.battery3.status.cell_balancing_status[i] ? "true," : "false,";
      }
      content += "];";

      content += "const min_mv3 = Math.min(...data3) - 30;";
      content += "const max_mv3 = Math.max(...data3) + 30;";
      content += "const min_index3 = data3.indexOf(Math.min(...data3));";
      content += "const max_index3 = data3.indexOf(Math.max(...data3));";
      content += "const graphContainer3 = document.getElementById('graph3');";
      content += "const valueDisplay3 = document.getElementById('valueDisplay3');";
      content += "const cellContainer3 = document.getElementById('cellContainer3');";

      // Arduino-style map() function
      content +=
          "function map3(value, fromLow, fromHigh, toLow, toHigh) {return (value - fromLow) * (toHigh - toLow) / "
          "(fromHigh - fromLow) + toLow;}";

      // Mark cell and bar with highest/lowest values
      content +=
          "function checkMinMax3(cell3, bar3, index3) {if ((index3 == min_index3) || (index3 == max_index3)) "
          "{cell3.style.borderColor = 'red';bar3.style.borderColor = 'red';}}";

      // Bar function. Basically get the mV, scale the height and add a bar div to its container
      content +=
          "function createBars3(data3) {"
          "data3.forEach((mV, index3) => {"
          "const bar3 = document.createElement('div');"
          "const mV_limited3 = map3(mV, min_mv3, max_mv3, 30, 300);"
          "bar3.className = 'bar';"
          "bar3.id = `barIndex3${index3}`;"
          "bar3.style.height = `${mV_limited3}px`;"
          "bar3.style.width = `${750/data3.length}px`;"
          "if (balancing3[index3]) {"
          "  bar3.style.backgroundColor = '#00FFFF';"  // Cyan color for balancing
          "  bar3.style.borderColor = '#00FFFF';"
          "} else {"
          "  bar3.style.backgroundColor = 'blue';"  // Normal blue for non-balancing
          "  bar3.style.borderColor = 'white';"
          "}"
          "const cell3 = document.getElementById(`cellIndex3${index3}`);"

          "checkMinMax3(cell3, bar3, index3);"

          "bar3.addEventListener('mouseenter', () => {"
          "    valueDisplay3.textContent = `Value: ${mV}` + (balancing[index3] ? ' (balancing)' : '');"
          "    bar3.style.backgroundColor = balancing3[index3] ? '#80FFFF' : 'lightblue';"
          "    cell3.style.backgroundColor = balancing3[index3] ? '#006666' : 'blue';"
          "});"

          "bar3.addEventListener('mouseleave', () => {"
          "valueDisplay3.textContent = 'Value: ...';"
          "bar3.style.backgroundColor = balancing3[index3] ? '#00FFFF' : 'blue';"  // Restore cyan if balancing, else blue
          "cell3.style.removeProperty('background-color');"
          "});"

          "graphContainer3.appendChild(bar3);"
          "});"
          "}";

      // Cell population function. For each value, add a cell block with its value
      content +=
          "function createCells3(data3) {"
          "data3.forEach((mV, index3) => {"
          "const cell3 = document.createElement('div');"
          "cell3.className = 'cell';"
          "cell3.id = `cellIndex3${index3}`;"
          "let cellContent3 = `Cell ${index3 + 1}<br>${mV} mV`;"
          "if (mV < 3000) {"
          "cellContent3 = `<span class='low-voltage'>${cellContent3}</span>`;"
          "}"
          "cell3.innerHTML = cellContent3;"

          "cell3.addEventListener('mouseenter', () => {"
          "let bar3 = document.getElementById(`barIndex3${index3}`);"
          "valueDisplay3.textContent = `Value: ${mV}`;"
          "bar3.style.backgroundColor = balancing3[index3] ? '#80FFFF' : 'lightblue';"  // Lighter cyan if balancing
          "cell3.style.backgroundColor = balancing3[index3] ? '#006666' : 'blue';"      // Darker cyan if balancing
          "});"

          "cell3.addEventListener('mouseleave', () => {"
          "let bar3 = document.getElementById(`barIndex3${index3}`);"
          "bar3.style.backgroundColor = balancing3[index3] ? '#00FFFF' : 'blue';"  // Restore original color
          "cell3.style.removeProperty('background-color');"
          "});"

          "cellContainer3.appendChild(cell3);"
          "});"
          "}";

      // On fetch, update the header of max/min/deviation client-side for consistency
      content +=
          "function updateVoltageValues3(data3) {"
          "const min_mv3 = Math.min(...data3);"
          "const max_mv3 = Math.max(...data3);"
          "const cell_dev3 = max_mv3 - min_mv3;"
          "const voltVal3 = document.getElementById('voltageValues3');"
          "voltVal3.innerHTML = `Battery #3<br>Max Voltage : ${max_mv3} mV<br>Min Voltage: ${min_mv3} mV<br>Voltage "
          "Deviation: "
          "${cell_dev3} mV`"
          "}";

      // If we have values, do the thing. Otherwise, display friendly message and wait
      content += "if (data3.length != 0) {";
      content += "createCells3(data3);";
      content += "createBars3(data3);";
      content += "updateVoltageValues3(data3);";
      content += "}";
      content += "else {";
      if (datalayer.battery3.info.number_of_cells > 0) {
        content += "document.getElementById('voltageValues3').textContent = '" +
                   String(datalayer.battery3.info.number_of_cells) +
                   " cells configured, but cellvoltages not yet read';";
      } else {
        content +=
            "document.getElementById('voltageValues3').textContent = 'Amount of cells unknown. Cellvoltages not yet "
            "read';";
      }
      content += "}";
    }

    // Automatic refresh is nice
    content += "setTimeout(function(){ location.reload(true); }, 20000);";

    content += "</script>";
    return content;
  }
  return String();
}
