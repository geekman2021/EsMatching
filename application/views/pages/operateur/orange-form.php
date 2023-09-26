
<?php // print_r($_SESSION["userExist"]) ; ?>

<form id="addform" method="post"  action="<?php echo site_url("orange/importdata"); ?>" enctype="multipart/form-data" >
    <div class="container mt-5">
        <div class="row">
            <div class="form-group col-lg-6">
                <label for="orange">Orange</label>
                <input type="file" class="form-control" id="input1" name="orange" onchange="checkFileExtension('input1')" required >
                <p id="warning1" class="warning-message"></p>
            </div>
            <div class="form-group col-lg-6">
                <label for="com">Commission</label>
                <input type="file" class="form-control" id="input2" name="com" onchange="checkFileExtension('input2')" required>
                <p id="warning2" class="warning-message"></p>
            </div>
            <div class="form-group col-lg-12">
                <label for="princ">Principale</label>
                <input type="file" class="form-control" id="input3" name="princ" onchange="checkFileExtension('input3')" required>
                <p id="warning3" class="warning-message"></p>
            </div>

        </div>
        <div class="container">
            <div class="text-center">
                <button type="reset" class="btn btn-warning" id="btn-reinitialiser">Reinitialiser</button>
                <button type="submit" class="btn btn-success" id="btn-ajouter">Comparer ces deux fichiers</button>
            </div>
        </div>
    </div>
</form>

<script>

function checkFileExtension(inputId) {
    var input = document.getElementById(inputId);
    var file = input.files[0];
    var fileName = file.name;
    var fileExtension = fileName.split('.').pop().toLowerCase();
    var warningMessage = document.getElementById('warning' + inputId.charAt(inputId.length - 1)); // Obtenez l'élément de message d'avertissement correspondant

    if (fileExtension === 'xls' || fileExtension === 'csv' || fileExtension=== 'xlsx' ) {
        warningMessage.textContent = '';
        warningMessage.style.display = 'none';
        document.getElementById("btn-ajouter").disabled= false;
    } else {
        warningMessage.textContent = 'Le fichier doit avoir une extension .xls';
        let element= $(".warning-message");

        element.css({
            'color': 'red',
            'font-weight': 'bold',
            'margin-top': '10px',
            'display': 'none'
        });
        warningMessage.style.display = 'block';
        input.value = '';
        document.getElementById("btn-ajouter").disabled= true;
    }



}

    $(document).ready(function() {
        // var inputfile= document.getElementById("orange");
        // var inputfile= document.getElementById("com");
        // var inputfile= document.getElementById("princ");

        // inputfile.addEventListener("change", function(e) {
        //     e.preventDefault();
        //     const fileName= this.value;
        //     if(fileName) {
        //         const extension = fileName.split(".").pop().toLowerCase();
        //         if(extension ==="csv" || extension === "xls" || extension === "xlsx") {
        //             document.getElementById("btn-ajouter").disabled = false;
        //         } else if (extension !== "csv" || extension !== "xls" || extension !== "xlsx") {
        //             document.getElementById("btn-ajouter").disabled = true;
        //         } else {
        //             document.getElementById("btn-ajouter").disabled = true;
                    
        //         }
        //     }
        // });

        // function checkFileExtension(inputId) {
        //     alert("hey");
        //     var input = document.getElementById(inputId);
        //     var file = input.files[0];
        //     var fileName = file.name;
        //     var fileExtension = fileName.split('.').pop().toLowerCase();
        //     var warningMessage = document.getElementById('warning' + inputId.charAt(inputId.length - 1)); // Obtenez l'élément de message d'avertissement correspondant

        //     if (fileExtension !== 'xls') {
        //         warningMessage.textContent = 'Le fichier doit avoir une extension .xls';
        //         warningMessage.style.display = 'block';
        //         input.value = '';

        //     } else {
        //         warningMessage.textContent = '';
        //         warningMessage.style.display = 'none';
        //     }

        // }

    })
    
</script> 