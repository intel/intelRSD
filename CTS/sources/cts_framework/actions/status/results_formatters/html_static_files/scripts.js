function PanelColorChange() {
    var overallStatus = document.getElementById('overallStatus').innerText;
    var panelHeading = document.getElementById('panelHeading');

    if (overallStatus == "PASSED") {
        panelHeading.className += " panel-success";
    } else if (overallStatus == "FAILED") {
        panelHeading.className += " panel-danger";
    } else {
        panelHeading.className += " panel-info"
    }
}

function UpdateBadgeValue() {
    var badgeValues = document.getElementById('passingRateBadge').innerText.split('/');
    var badge = document.getElementById('passingRateBadge');

    if (badgeValues[0] == badgeValues[1]) {
        badge.className += " badge-success";
    } else if (badgeValues[0] == 0) {
        badge.className += " badge-failed";
    }
     else {
        badge.className += " badge-warning";
    }
}


function filter(id, name) {
    var btnstatus = document.getElementById(id);
    var x = document.getElementsByClassName(name);

    x = [].slice.call(x);
    console.log(btnstatus.className.indexOf("disabled"));

    if(btnstatus.className.indexOf("disabled") == -1){
        if (btnstatus.className.indexOf("active") >= 0) {
            btnstatus.className = btnstatus.className.replace("active", "");
            x.forEach(function (element) {
                element.style.display = "none";
            });
        } else {
            btnstatus.className += "active";
            x.forEach(function (element) {
                element.style.display = "block";
            });
        }
    }
}

PanelColorChange();
UpdateBadgeValue();