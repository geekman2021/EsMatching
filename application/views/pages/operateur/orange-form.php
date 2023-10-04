
<?php // print_r($_SESSION["userExist"]) ; ?>

<form id="addform" method="post"  action="<?php echo site_url("importer_orange/importer"); ?>" enctype="multipart/form-data" >
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

<div id="progress-container" style="display: none;">
    <div id="progress-bar" class="progress" style="height: 30px; background-color: #007bff; color: #fff;">
        <div class="progress-bar progress-bar-striped active" role="progressbar" aria-valuenow="0" aria-valuemin="0" aria-valuemax="100">
            0%
        </div>
    </div>
</div>



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
        $("#addform").submit(function(event) {
            event.preventDefault();
            $("#progress-container").show();
            $.ajax({
                url: $(this).attr("action"),
                type: $(this).attr("method"),
                data: new FormData(this),
                contentType: false,
                processData: false,
                xhr: function() {
                    var xhr = new window.XMLHttpRequest();

                    xhr.upload.addEventListener("progress", function(evt) {
                        if (evt.lengthComputable) {
                            var percentComplete = (evt.loaded / evt.total) * 100;
                            $("#progress-bar").attr("aria-valuenow", percentComplete).css("width", percentComplete + "%").text(percentComplete.toFixed(2) + "%");
                        }
                    }, false);

                    return xhr;
                },
                success: function(response) {
                    console.log(response);
                    $("#progress-container").hide();
                    swal({
                        title: "Succès",
                        text: "Données importer avec succès",
                        icon: "success",
                        button: "OK",
                    });
                },
                error: function() {
                    alert("Une erreur s'est produite lors de l'envoi du formulaire.");
                }
            });
        });

    })
    
</script> 