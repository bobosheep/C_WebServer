let left = document.getElementById("left-control")
let right = document.getElementById("right-control")
let nowpic = 1

function changePicture(control){
    if(control === "left"){
        nowpic = nowpic > 1 ? nowpic - 1 : 5
    }
    else{
        nowpic = nowpic <= 4 ? nowpic + 1 : 1;
    }
    jpg = nowpic === 5 ? ".jpg": ".jpeg"
    document.getElementById("gallery").src = "img/img" + nowpic + jpg
}